/*
 Goblin - An Ogre3D Utility Library
 Copyright (C) DNTeam <goblin@dnteam.org>
 
 This file is part of Goblin.
 
 Goblin is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Goblin is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with Goblin.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "camera.h"
#include "screeninfo.h"

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   #include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   #include <kobold/multitouchcontroller.h>
#else
   #include <kobold/mouse.h>
   #include <kobold/keyboard.h>
#endif


#if ((OGRE_VERSION_MAJOR == 1) ||\
     (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR <= 1))
   #include <OGRE/OgreViewport.h>
#endif

#include <kobold/log.h>

namespace Goblin
{

/***********************************************************************
 *                            Constructor                              *
 ***********************************************************************/
CameraConfig::CameraConfig()
{
   this->linearAttenuation = 0.03f;
   this->angularAttenuation = 0.25f;
   this->zoomMin = 30.0f;
   this->zoomMax = 4.0f;
   this->linearVelocity = 0.15f;
   this->angularVelocity = 2.0f;
   this->zoomVelocity = 0.25f;
   this->nearClipDistance = 0.2f;
   this->farClipDistance = 600.0f;
   this->translateFactor = 0.01f;
   this->maxDistToTranslate = 1.6f;
   this->rotateFactor = 0.25f;
   this->thetaMax = 89.0f;
   this->thetaMin = 1.0f;
}

/***********************************************************************
 *                               assign                                *
 ***********************************************************************/
CameraConfig& CameraConfig::operator=(CameraConfig conf)
{
   this->linearAttenuation = conf.linearAttenuation;
   this->angularAttenuation = conf.angularAttenuation;
   this->zoomMin = conf.zoomMin;
   this->zoomMax = conf.zoomMax;
   this->linearVelocity = conf.linearVelocity;
   this->angularVelocity = conf.angularVelocity;
   this->zoomVelocity = conf.zoomVelocity;
   this->nearClipDistance = conf.nearClipDistance;
   this->farClipDistance = conf.farClipDistance;
   this->translateFactor = conf.translateFactor;
   this->maxDistToTranslate = conf.maxDistToTranslate;
   this->rotateFactor = conf.rotateFactor;
   this->thetaMax = conf.thetaMax;
   this->thetaMin = conf.thetaMin;

   return *this;
}

/***********************************************************************
 *                            Constructor                              *
 ***********************************************************************/
Camera::Camera()
{
}

/***********************************************************************
 *                               init                                  *
 ***********************************************************************/
void Camera::init(Ogre::SceneManager* ogreSceneManager, 
#if (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2)
      Ogre::Window* ogreRenderWindow,
#else
      Ogre::RenderWindow* ogreRenderWindow, 
#endif
      const CameraConfig& conf) 
{
   config = conf;

   state.phi = 0;
   state.theta = 55;
   state.zoom = (conf.zoomMin - conf.zoomMax) / 2;

   phiAc = 0;
   thetaAc = 0;
   zoomAc = 0;

   state.center.x = 0;
   state.center.y = 3.0f;
   state.center.z = 0.0f;

   if(ScreenInfo::shouldUseDoubleSizedGui())
   {
      /* Screen is too big, we should translate more for each finger
       * variation to not move too slow. */
      config.translateFactor *= 2.0f;
      config.maxDistToTranslate *= 2.0f;
   }
  
   centerXAc = 0.0f;
   centerYAc = 0.0f;
   centerZAc = 0.0f;
   
   initialDistance = CAMERA_UNDEFINED;
   needUpdate = false;
      
   /* Create the ogre Camera */
   ogreCamera = ogreSceneManager->createCamera("OGRE_Game_Camera");
   lookAt();
   ogreCamera->setNearClipDistance(config.nearClipDistance);
   ogreCamera->setFarClipDistance(config.farClipDistance);
   ogreCamera->setAutoAspectRatio(true);

#if (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR > 1)
   ogreCamera->setAspectRatio(Ogre::Real(ogreRenderWindow->getWidth()) / 
                              Ogre::Real(ogreRenderWindow->getHeight()));
#else
   /* And create the viewport */
   Ogre::Viewport* ogreViewport=NULL;
   #if OGRE_VERSION_MAJOR == 1
      ogreViewport = ogreRenderWindow->addViewport(ogreCamera);
      ogreViewport->setBackgroundColour(
            Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f));
   #else
      ogreViewport = ogreRenderWindow->addViewport();
   #endif

   ogreCamera->setAspectRatio(Ogre::Real(ogreViewport->getActualWidth()) / 
                              Ogre::Real(ogreViewport->getActualHeight()));
   #if OGRE_VERSION_MAJOR == 1
      ogreViewport->setCamera(ogreCamera);
   #endif
   //ogreCamera->setLodBias(1000.0f);

   #if OGRE_VERSION_MAJOR == 1 || \
       (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
      ogreViewport->setMaterialScheme(
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);   
      Ogre::RTShader::ShaderGenerator::getSingletonPtr()->invalidateScheme(
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
   #endif

#endif
                                                      
}

/***********************************************************************
 *                            limitValue                               *
 ***********************************************************************/
Ogre::Real Camera::limitValue(Ogre::Real v, Ogre::Real min, Ogre::Real max)
{
   if(v < min)
   {
      v = min;
   }
   else if(v > max)
   {
      v = max;
   }
   
   return v;
}

/***********************************************************************
 *                                  set                                *
 ***********************************************************************/
void Camera::set(Ogre::Real x, Ogre::Real y, Ogre::Real z, 
      Ogre::Real p, Ogre::Real t, Ogre::Real zo)
{
   /* Set values */
   setPosition(Ogre::Vector3(x, y, z), false);
   state.phi = rangeValue(p);
   state.theta = t;
   state.zoom = zo;

   /* Remove all targets and acelerations */
   centerXAc = 0.0f;
   centerYAc = 0.0f;
   centerZAc = 0.0f;
   phiAc = 0.0f;
   zoomAc = 0.0f;
   thetaAc = 0.0f;
   needUpdate = false;

   /* finally do the lookat */
   lookAt();
}

/***********************************************************************
 *                           setPosition                               *
 ***********************************************************************/
void Camera::setPosition(Ogre::Vector3 pos, bool doLookAt)
{
   float x = pos.x;
   float y = pos.y;
   float z = pos.z;

   /* Verify limits */
   if(limitedArea)
   {
      x = limitValue(x, minArea.x, maxArea.x);
      y = limitValue(y, minArea.y, maxArea.y);
      z = limitValue(z, minArea.z, maxArea.z);
   }
   
   /* Set values */
   state.center.x = x;
   state.center.y = y;
   state.center.z = z;

   if(doLookAt)
   {
      lookAt();
   }
}

/***********************************************************************
 *                             setTarget                               *
 ***********************************************************************/
void Camera::setTarget(Ogre::Real x, Ogre::Real y, Ogre::Real z, 
            Ogre::Real p, Ogre::Real t, Ogre::Real zo)
{
   setTarget(x, y, z, p, t, zo, config.linearVelocity, config.angularVelocity,
             config.zoomVelocity);
}

/***********************************************************************
 *                             setTarget                               *
 ***********************************************************************/
void Camera::setTarget(Ogre::Real x, Ogre::Real y, Ogre::Real z, 
            Ogre::Real p, Ogre::Real t, Ogre::Real zo, 
            Ogre::Real linearVelocity, Ogre::Real angularVelocity,
            Ogre::Real zoomVelocity)
{
   /* Set current values at Target */
   flushCurrentAtTarget();
   
   /* Verify limits */
   if(limitedArea)
   {
      x = limitValue(x, minArea.x, maxArea.x);
      y = limitValue(y, minArea.y, maxArea.y);
      z = limitValue(z, minArea.z, maxArea.z);
   }

   /* Define targets */
   targetX.setTargetAndVelocity(x, linearVelocity);
   targetY.setTargetAndVelocity(y, linearVelocity);
   targetZ.setTargetAndVelocity(z, linearVelocity);
 
   targetPhi.setTargetAndVelocity(rangeValue(p), angularVelocity);
   targetTheta.setTargetAndVelocity(t, angularVelocity);
   targetZoom.setTargetAndVelocity(zo, zoomVelocity);

   needUpdate = true;
}

/***********************************************************************
 *                         flushCurrentAtTarget                        *
 ***********************************************************************/
void Camera::flushCurrentAtTarget()
{
   targetX.setCurrent(state.center.x);
   targetY.setCurrent(state.center.y);
   targetZ.setCurrent(state.center.z);

   targetPhi.setCurrent(state.phi);
   targetTheta.setCurrent(state.theta);
   targetZoom.setCurrent(state.zoom);
}

/***********************************************************************
 *                                 push                                *
 ***********************************************************************/
void Camera::push()
{
   /* Save the state */
   prevState = state;
}

/***********************************************************************
 *                                 pop                                 *
 ***********************************************************************/
void Camera::pop(bool asTarget)
{
   if(asTarget)
   {
      /* Set target to the previous camera position */
      setTarget(prevState.center.x, prevState.center.y, prevState.center.z,
                prevState.phi, prevState.theta, prevState.zoom);
   }
   else
   {
      /* Get previous state and set it immediatly. */
      state = prevState;
      /* Do the lookAt */
      lookAt();
   }
}

/***********************************************************************
 *                                lookAt                               *
 ***********************************************************************/
void Camera::lookAt()
{
   Ogre::Radian thetaR = Ogre::Radian(Ogre::Degree(state.theta));
   Ogre::Radian phiR = Ogre::Radian(Ogre::Degree(state.phi));

   Ogre::Real cosTheta = Ogre::Math::Cos(thetaR);
   Ogre::Real sinTheta = Ogre::Math::Sin(thetaR);

   Ogre::Real cosPhi = Ogre::Math::Cos(phiR);
   Ogre::Real sinPhi = Ogre::Math::Sin(phiR);

   eye.x = state.center.x + state.zoom * cosTheta * sinPhi;
   eye.y = state.center.y + state.zoom * sinTheta;
   eye.z = state.center.z + state.zoom * cosTheta * cosPhi;

   ogreCamera->setPosition(eye);
   ogreCamera->lookAt(state.center);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
/***********************************************************************
 *                       verifyMultiTouchInput                         *
 ***********************************************************************/
bool Camera::verifyMultiTouchInput()
{
   Kobold::TouchInfo p0, p1, p2;
   Kobold::TouchInfo* pt;
   Ogre::Real curDist;
   
   if(Kobold::MultiTouchController::totalTouches() == 2)
   {
      /* Get touches */
      Kobold::MultiTouchController::getTouch(0, p0);
      Kobold::MultiTouchController::getTouch(1, p1);
      
      /* Calculate the current squared distance */
      curDist = Ogre::Math::Sqr(p0.x - p1.x) + 
                Ogre::Math::Sqr(p0.y - p1.y);
      curDist = Ogre::Math::Sqrt(curDist);

      if( (Ogre::Math::isNaN(curDist)) || (curDist == 0.0f) ||
          (curDist >= CAMERA_MAX_TOUCHES_DISTANCE) )
      {
         /* Calculated a nan value, must stop (or greater than max,
          * or same finger positions!) Aborting, (but returning true, 
          * as it was a potential camera move, treatned here) */
         return true;
      }
      
      /* Set initial, if needed */
      if(initialDistance == CAMERA_UNDEFINED)
      {
         initialDistance = curDist;
      }
      /* Otherwise, do the zoom or camera move */
      else if( (p0.state == TOUCH_MOVED) ||
               (p1.state == TOUCH_MOVED) )
      {
         /* Verify if the distance is somewhat constant */
         if(Ogre::Math::Abs(curDist - initialDistance) <= 
               config.maxDistToTranslate)
         {
            /* Is constant: moving the camera center, if enabled */
            if(!canTranslate)
            {
               /* Can't translate the camera. */
               return false;
            }
            Ogre::Real dX = p0.prevX - p0.x;
            Ogre::Real dZ = p0.prevY - p0.y;
            Ogre::Real varXAc = 0.0f;
            Ogre::Real varZAc = 0.0f;

            if(dZ != 0)
            {
               /* Try to move Forward/Backward to camera facing position */
               varXAc += dZ * config.translateFactor * 
                  Ogre::Math::Sin(Ogre::Radian(Ogre::Degree(state.phi)));
               varZAc += dZ * config.translateFactor * 
                  Ogre::Math::Cos(Ogre::Radian(Ogre::Degree(state.phi)));
            }
            if(dX != 0)
            {
               /* Try to move Left/Right to camera facing position */
               varXAc += dX * config.translateFactor *
                  Ogre::Math::Sin(Ogre::Radian(Ogre::Degree(state.phi+90)));
               varZAc += dX * config.translateFactor * 
                  Ogre::Math::Cos(Ogre::Radian( Ogre::Degree(state.phi+90)));
            }
            
            if( (Ogre::Math::isNaN(varXAc)) || (Ogre::Math::isNaN(varZAc)) )
            {
               /* Translating by NaN, must abort translation. */
               initialDistance = CAMERA_UNDEFINED;
               return true;
            }
            
            /* Apply the acceleration move */
            if((varXAc != 0) || (varZAc != 0))
            {
               centerXAc = varXAc;
               centerZAc = varZAc;
            }
         
         }
         else
         {
            /* Distance changed. Do the zoom change, if enabled */
            if(!canZoom)
            {
               /* Can't translate the camera. */
               return false;
            }
            if(Ogre::Math::isNaN(initialDistance))
            {
               /* Zoom by NaN, must abort it. */
               initialDistance = CAMERA_UNDEFINED;
               return true;
            }
            zoomAc = ((initialDistance - curDist) / config.maxDistToTranslate);
         }
      }
      
      return true;
   }
   else
   {
      /* No distance beetween points */
      initialDistance = CAMERA_UNDEFINED;
      
      if( (Kobold::MultiTouchController::totalTouches() == 3) && (canRotate) )
      {
         if((p0.equalPosition(p1)) || (p1.equalPosition(p2)) || 
            (p0.equalPosition(p2))) 
         {
            /* Equal fingers? Must do nothing, as not real 3 fingers
             * avaiable. */
            return false;
         }

         /* Let's treat rotation */
         Kobold::MultiTouchController::getTouch(0, p0);
         Kobold::MultiTouchController::getTouch(1, p1);
         Kobold::MultiTouchController::getTouch(2, p2);
         
         /* Get which is the orientation finger */
         pt = NULL;
         if( (p0.state != TOUCH_MOVED) &&
             (p1.state != TOUCH_MOVED) &&
             (p2.state == TOUCH_MOVED) )
         {
            pt = &p2;
         }
         else if( (p0.state != TOUCH_MOVED) &&
                  (p1.state == TOUCH_MOVED) &&
                  (p2.state != TOUCH_MOVED))
         {
            pt = &p1;
         }
         else if( (p0.state == TOUCH_MOVED) &&
                  (p1.state != TOUCH_MOVED) &&
                  (p2.state != TOUCH_MOVED))
         {
            pt = &p0;
         }
         
         if(pt != NULL)
         {
            /* Set Rotation Acceleration, according to the rotation finger */
            Ogre::Real dX = pt->prevX - pt->x;
            Ogre::Real dY = pt->prevY - pt->y;
            
            if( (Ogre::Math::isNaN(dX)) || (Ogre::Math::isNaN(dY)) )
            {
               /* Rotating by NaN, must abort rotation. */
               return false;
            }
            
            if(dX != 0)
            {
               phiAc = dX * config.rotateFactor;;
            }
            if(dY != 0)
            {
               thetaAc = dY * config.rotateFactor;
            }
         }
      }
   }
   
   return false;
}
#else
/***********************************************************************
 *                          verifyMouseInput                           *
 ***********************************************************************/
bool Camera::verifyMouseInput()
{
   /* Middle Mouse Button Rotation Control */
   if((Kobold::Mouse::isMiddleButtonPressed()) &&
      ((Kobold::Mouse::getRelativeY() != 0) ||
       (Kobold::Mouse::getRelativeX() != 0)))
   {
      /* Do the move */
      thetaAc = (Kobold::Mouse::getRelativeY()) * config.rotateFactor;
      phiAc = (Kobold::Mouse::getRelativeX()) * config.rotateFactor;

      /* Limit rotation accelaration, to give vamera some 'inertia' 
       * (more fluid movement) */
      if(Ogre::Math::Abs(thetaAc) > 5.0f)
      {
         int signal = (thetaAc > 0.0f) ? 1.0f : -1.0f;
         thetaAc = signal * 5.0f;
      }
      if(Ogre::Math::Abs(phiAc) > 5.0f)
      {
         int signal = (phiAc > 0.0f) ? 1.0f : -1.0f;
         phiAc = signal * 5.0f;
      }

      return true;
   }

   /* Verify mouse wheel */
   if(Kobold::Mouse::getRelativeWheel() != 0)
   {
      /* Inverted! */
      zoomAc -= (Kobold::Mouse::getRelativeWheel() * 
                    config.zoomVelocity * 0.5f);
      return true;
   }

   return false;
}

/***********************************************************************
 *                         verifyKeyboardInput                         *
 ***********************************************************************/
bool Camera::verifyKeyboardInput()
{
   bool moved = false;
   int varCamera = 1.0f;
   Ogre::Real varXAc = 0.0f;
   Ogre::Real varZAc = 0.0f;

   /* Double Camera speed when shift pressed */
   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_RSHIFT)) || 
         (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_LSHIFT)) )
   {
      varCamera *= 2;
   }

   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_UP)) && (canZoom) )
   {
      /* Increase zoom */
      zoomAc = -varCamera * config.zoomVelocity;
      moved = true;
   }
   else if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_DOWN)) && 
            (canZoom) )
   {
      /* Decrease zoom */
      zoomAc = varCamera * config.zoomVelocity;
      moved = true;
   }

   if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_HOME)) ||
         (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_7)) ) &&
      (canZoom) ) 
   {
      /* Maximize zoom */
      zoomAc = -10 * config.zoomVelocity;
      moved = true;
   }
   else if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_END)) ||
              (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_1) ) ) &&
            (canZoom) )
   {
      /* Minimize zoom */
      zoomAc = 10 * config.zoomVelocity;
      moved = true;
   }

   if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_INSERT)) ||
         (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_0)) ) &&
       (canRotate) )
   {
      /* Max Up */
      thetaAc = 10;
      moved = true;
   }
   else if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_DELETE)) ||
              (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_DECIMAL)) 
            ) && (canRotate) )
   {
      /* Max Down */
      thetaAc = -10;
      moved = true;
   }

   if(Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_F5))
   {
      Kobold::Log::add(Kobold::LOG_LEVEL_NORMAL, 
            "Center: (%.2f, %.2f, %.2f)\nPhi: %.2f Theta: %.2f Zoom: %.2f\n",
            state.center.x, state.center.y,state.center.z, 
            state.phi, state.theta, state.zoom);
   }

   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_LEFT)) && 
       (canRotate) )
   {
      /* Rotate left */
      phiAc = varCamera * config.angularVelocity;
      moved = true;
   }
   else if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_RIGHT)) && 
            (canRotate) )
   {
      /* Rotate Right */
      phiAc = -varCamera * config.angularVelocity;
      moved = true;
   }

   if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_PAGEUP)) ||
         (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_9)) ) &&
       (canRotate) )
   {
      /* Rotate Up */
      thetaAc = varCamera * config.angularVelocity;
      moved = true;
   }
   else if( ( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_PAGEDOWN)) ||
         (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_3)) ) &&
         (canRotate) )
   {
      /* Rotate Down */
      thetaAc = -varCamera * config.angularVelocity;
      moved = true;
   }

   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_8)) && 
       (canTranslate) )
   {
      /* Move Forward */
      varXAc += -varCamera*Ogre::Math::Sin(Ogre::Radian(
               Ogre::Degree(state.phi)))*config.linearVelocity;
      varZAc += -varCamera*Ogre::Math::Cos(Ogre::Radian(
               Ogre::Degree(state.phi)))*config.linearVelocity;
      moved = true;
   }
   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_2)) && 
       (canTranslate) )
   {
      /* Move Backward */
      varXAc += varCamera*Ogre::Math::Sin(Ogre::Radian(
               Ogre::Degree(state.phi)))*config.linearVelocity;
      varZAc += varCamera*Ogre::Math::Cos(Ogre::Radian(
               Ogre::Degree(state.phi)))*config.linearVelocity;
      moved = true;
   }

   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_4)) && 
       (canTranslate) )
   {
      /* Move Left */
      varXAc += -varCamera*Ogre::Math::Sin(Ogre::Radian(
               Ogre::Degree(state.phi+90)))*config.linearVelocity;
      varZAc += -varCamera*Ogre::Math::Cos(Ogre::Radian(
               Ogre::Degree(state.phi+90)))*config.linearVelocity;
      moved = true;
   }
   if( (Kobold::Keyboard::isKeyPressed(Kobold::KOBOLD_KEY_KP_6)) && 
       (canTranslate) )
   {
      /* Move Right */
      varXAc += varCamera*Ogre::Math::Sin(Ogre::Radian(
               Ogre::Degree(state.phi+90)))*config.linearVelocity;
      varZAc += varCamera*Ogre::Math::Cos(Ogre::Radian(
               Ogre::Degree(state.phi+90)))*config.linearVelocity;
      moved = true;
   }

   /* Apply acceleration with varX and varZ do possible simultaneous
    * directional move (up and left, for example) */
   if((varXAc != 0) || (varZAc != 0))
   {
      centerXAc = varXAc;
      centerZAc = varZAc;
   }

   return moved;
}
#endif

/***********************************************************************
 *                                doMove                               *
 ***********************************************************************/
bool Camera::doMove() 
{
   bool moved = false;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID

   /* Verify MultiTouch input */
   moved |= verifyMultiTouchInput();

#else

   /* Verify keyboard and mouse input */
   moved |= verifyKeyboardInput();
   moved |= verifyMouseInput();

#endif

   moved |= applyAccelerationsAndMove();

   return moved;
}

/***********************************************************************
 *                       applyAccelerationAndMove                      *
 ***********************************************************************/
bool Camera::applyAccelerationsAndMove()
{
   bool moved = false;

   /* Verify if have some Target to go to */
   if(needUpdate)
   {
      /* Have target, must update to it and ignore input (acceletarions) */
      targetTheta.update();
      state.theta = targetTheta.getValue();
      targetZoom.update();
      state.zoom = targetZoom.getValue();
      targetPhi.update();
      state.phi = targetPhi.getValue();

      targetX.update();
      state.center.x = targetX.getValue();
      targetY.update();
      state.center.y = targetY.getValue();
      targetZ.update();
      state.center.z = targetZ.getValue();

      /* Update the need update-thing */
      needUpdate = targetX.needUpdate() || targetY.needUpdate() ||
                   targetZ.needUpdate() || targetPhi.needUpdate() ||
                   targetTheta.needUpdate() || targetZoom.needUpdate();

      /* Make sure no other acceleration is seted */
      thetaAc = 0.0f;
      phiAc = 0.0f;
      zoomAc = 0.0f;
      centerXAc = 0.0f;
      centerYAc = 0.0f;
      centerZAc = 0.0f;

      /* Do the look-at */
      lookAt();

      return true;
   }


   
   /* Apply Phi acceleration */
   if(phiAc != 0)
   {
      applyAcceleration(state.phi, phiAc, config.angularAttenuation);
      state.phi = rangeValue(state.phi);
      moved = true;
   }
   /* Apply theta acceleration */
   if(thetaAc != 0)
   {
      applyAcceleration(state.theta, thetaAc, config.angularAttenuation);
      moved = true;
   }
   
   /* Apply zoom acceleration */
   if(zoomAc != 0)
   {
      applyAcceleration(state.zoom, zoomAc, config.linearAttenuation);
      moved = true;
   }
   
   /* Apply centerX Acceleration */
   if(centerXAc != 0)
   {
      if(limitedArea)
      {
         applyAcceleration(state.center.x, centerXAc, minArea.x, maxArea.x,
               config.linearAttenuation);
      }
      else
      {
         applyAcceleration(state.center.x, centerXAc, config.linearAttenuation);
      }
      moved = true;
   }
   /* Apply centerY Acceleration */
   if(centerYAc != 0)
   {
      if(limitedArea)
      {
         applyAcceleration(state.center.y, centerYAc, minArea.y, maxArea.y,
               config.linearAttenuation);
      }
      else
      {
         applyAcceleration(state.center.y, centerYAc, config.linearAttenuation);
      }
      moved = true;
   }
   /* Apply centerZ Acceleration */
   if(centerZAc != 0)
   {
      if(limitedArea)
      {
         applyAcceleration(state.center.z, centerZAc, minArea.z, maxArea.z, 
               config.linearAttenuation);
      }
      else
      {
         applyAcceleration(state.center.z, centerZAc, config.linearAttenuation);
      }
      moved = true;
   }
   
   /* Camera moved, must set the lookAt */
   if(moved)
   {
      /* Verify Limits */
      if(state.theta > config.thetaMax)
      {
         state.theta = config.thetaMax;
         thetaAc = 0;
      }
      else if(state.theta < config.thetaMin)
      {
         state.theta = config.thetaMin;
         thetaAc = 0;
      }
      if(state.zoom < config.zoomMax)
      {
         state.zoom = config.zoomMax;
         zoomAc = 0;
      }
      else if(state.zoom > config.zoomMin)
      {
         state.zoom = config.zoomMin;
         zoomAc = 0;
      }
      
      /* Do the lookUp */
      lookAt();
   }

   return moved;
}

/***********************************************************************
 *                          applyAcceleration                          *
 ***********************************************************************/
void Camera::applyAcceleration(Ogre::Real& factor, Ogre::Real& ac,
            const Ogre::Real& attenuation)
{
   /* Apply to the factor */
   factor += ac;

   /* Attenuate */
   if(ac > 0)
   {
      ac -= attenuation;
      if(ac < 0)
      {
         /* Must Stop */
         ac = 0;
      }
   }
   else
   {
      ac += attenuation;
      if(ac > 0)
      {
         ac = 0;
      }
   }
}

/***********************************************************************
 *                          applyAcceleration                          *
 ***********************************************************************/
void Camera::applyAcceleration(Ogre::Real& factor, Ogre::Real& ac,
                               Ogre::Real minValue, Ogre::Real maxValue,
                               const Ogre::Real& attenuation)
{
   /* Apply normally the accelleration */
   applyAcceleration(factor, ac, attenuation);
   
   /* Check limits, stopping if reached. */
   if(factor <= minValue)
   {
      factor = minValue;
      ac = 0.0f;
   }
   else if(factor >= maxValue)
   {
      factor = maxValue;
      ac = 0.0f;
   }
}

/***********************************************************************
 *                         getCameraToViewportRay                      *
 ***********************************************************************/
void Camera::getCameraToViewportRay(Ogre::Real x, Ogre::Real y, 
      Ogre::Ray* outRay)
{
   ogreCamera->getCameraToViewportRay(x, y, outRay);
}

/***********************************************************************
 *                                 setPhi                              *
 ***********************************************************************/
void Camera::setPhi(Ogre::Real value)
{
   flushCurrentAtTarget();
   targetPhi.setTargetAndVelocity(rangeValue(value), config.angularVelocity);
   needUpdate = true;
}
   
/***********************************************************************
 *                                 setPhi                              *
 ***********************************************************************/
void Camera::setCurrentPhi(Ogre::Real value)
{
   state.phi = value;
   flushCurrentAtTarget();
   needUpdate = true;
}

/***********************************************************************
 *                               isVisible                             *
 ***********************************************************************/
bool Camera::isVisible(Ogre::AxisAlignedBox bbox)
{
   return ogreCamera->isVisible(bbox);
}
   
/***********************************************************************
 *                           enableRotations                           *
 ***********************************************************************/
void Camera::enableRotations()
{
   canRotate = true;
}
   
/***********************************************************************
 *                         enableTranslations                          *
 ***********************************************************************/
void Camera::enableTranslations()
{
   canTranslate = true;
}
   
/***********************************************************************
 *                         enableZoomChanges                           *
 ***********************************************************************/
void Camera::enableZoomChanges()
{
   canZoom = true;
}
   
/***********************************************************************
 *                          disableRotations                           *
 ***********************************************************************/
void Camera::disableRotations()
{
   canRotate = false;
}
   
/***********************************************************************
 *                        disableTranslations                          *
 ***********************************************************************/
void Camera::disableTranslations()
{
   canTranslate = false;
}
   
/***********************************************************************
 *                        disableZoomChanges                           *
 ***********************************************************************/
void Camera::disableZoomChanges()
{
   canZoom = false;
}

/***********************************************************************
 *                           rangeValue                                *
 ***********************************************************************/
Ogre::Real Camera::rangeValue(Ogre::Real v)
{
   while(v < -180)
   {
      v += 360;
   }
   
   while(v > 180)
   {
      v -= 360;
   }
   
   return v;
}

/***********************************************************************
 *                        limitCameraArea                              *
 ***********************************************************************/
void Camera::limitCameraArea(Ogre::Vector3 min, Ogre::Vector3 max)
{
   limitedArea = true;
   minArea = min;
   maxArea = max;
}

/***********************************************************************
 *                     removeCameraAreaLimits                          *
 ***********************************************************************/
void Camera::removeCameraAreaLimits()
{
   limitedArea = false;
}

/***********************************************************************
 *                            getConfiguration                         *
 ***********************************************************************/
CameraConfig Camera::getConfiguration()
{
   return config;
}

/***********************************************************************
 *                               Static Fields                         *
 ***********************************************************************/
Ogre::Camera* Camera::ogreCamera=NULL;

CameraState Camera::state;
CameraState Camera::prevState;

Ogre::Real Camera::phiAc=0.0f;
Ogre::Real Camera::thetaAc=0.0f;
Ogre::Real Camera::zoomAc=0.0f;

Kobold::Target Camera::targetPhi;
Kobold::Target Camera::targetTheta;
Kobold::Target Camera::targetZoom;
Kobold::Target Camera::targetX;
Kobold::Target Camera::targetY;
Kobold::Target Camera::targetZ;

Ogre::Vector3 Camera::eye;

Ogre::Real Camera::centerXAc=0.0f;
Ogre::Real Camera::centerYAc=0.0f;
Ogre::Real Camera::centerZAc=0.0f;
   
Ogre::Real Camera::initialDistance = CAMERA_UNDEFINED;
bool Camera::needUpdate = false;
   
bool Camera::canTranslate = true;
bool Camera::canRotate = true;
bool Camera::canZoom = true;
   
bool Camera::limitedArea = false;
Ogre::Vector3 Camera::minArea;
Ogre::Vector3 Camera::maxArea;

CameraConfig Camera::config;

}
