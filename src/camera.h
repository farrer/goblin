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

#ifndef _goblin_camera_h
#define _goblin_camera_h

/* Ogre headers */
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreViewport.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/OgreMath.h>
#include <OGRE/OgreRay.h>

#include <kobold/target.h>

namespace Goblin
{

#define CAMERA_ATTENUATION                    0.1f
#define CAMERA_ZOOM_MIN                     600.0f
#define CAMERA_ZOOM_MAX                      80.0f
#define CAMERA_UNDEFINED                 -10000.0f
#define CAMERA_MAX_TOUCHES_DISTANCE        5000.0f
#define CAMERA_DEFAULT_LINEAR_VELOCITY        5.0f
#define CAMERA_DEFAULT_ANGULAR_VELOCITY       8.0f
#define CAMERA_DEFAULT_ZOOM_VELOCITY         20.0f

#define CAMERA_DEFAULT_NEAR_CLIP_DISTANCE     2.2f
#define CAMERA_DEFAULT_FAR_CLIP_DISTANCE   2200.0f

/*! The camera single state (position and angles) */
class CameraState
{
   public:
      Ogre::Vector3 center;   /**< The camera center position */
      Ogre::Real phi;         /**< The camera phi angle */
      Ogre::Real theta;       /**< The camera theta angle */
      Ogre::Real zoom;        /**< The camera zoom angle */
};

/*! The Camera class implements an Isometric Camera Controller for ogre.
 * The Camera is defined by its center position (centerX, centerY, centerZ),
 * two orientation angles, phi for "side" rotation, theta for up/down rotation,
 * and a zoom value */
class Camera
{
   public:
      /*! Init the Camera to use
       * \param ogreSceneManager -> pointer to the used scene manager
       * \param ogreRenderWindow -> pointer to the used render window */
      static void init(Ogre::SceneManager* ogreSceneManager, 
            Ogre::RenderWindow* ogreRenderWindow, 
            Ogre::Real nearClip = CAMERA_DEFAULT_NEAR_CLIP_DISTANCE,
            Ogre::Real farClip = CAMERA_DEFAULT_FAR_CLIP_DISTANCE);

      /*! Instantaneous set Camera position/orientation */
      static void set(Ogre::Real x, Ogre::Real y, Ogre::Real z, 
            Ogre::Real p, Ogre::Real t, Ogre::Real zo);

      /*! Instantaneous set Camera to a new position 
       * \param pos new camera position
       * \param doLookAt if true, will define the lookAt, false,
       *        the definition will be done later. */
      static void setPosition(Ogre::Vector3 pos, bool doLookAt=true);

      /*! Set a Target position/orientation to the Camera
       * \note: Camera will follow a calculated path from current
       *        position/orientation to the Target defined here. */
      static void setTarget(Ogre::Real x, Ogre::Real y, Ogre::Real z, 
            Ogre::Real p, Ogre::Real t, Ogre::Real zo,
            Ogre::Real linearVelocity=CAMERA_DEFAULT_LINEAR_VELOCITY,
            Ogre::Real angularVelocity=CAMERA_DEFAULT_ANGULAR_VELOCITY,
            Ogre::Real zoomVelocity=CAMERA_DEFAULT_ZOOM_VELOCITY);

      /*! Do the Camera movimentation, based on mouse and keyboard events 
       * (common OS) or on multitouch events on iOS
       * \return true if updated Camera with event, false otherwise */
      static bool doMove();
      
      /*! Get the viewport ray from point x,y
       * \param x -> X coordinate on screen [0,1] 
       * \param y -> Y coordinate on screen [0,1] 
       * \param outRay -> pointer to keep the result Ogre::Ray from x,y */
      static void getCameraToViewportRay(Ogre::Real x, Ogre::Real y, 
            Ogre::Ray* outRay);
   
      static Ogre::Viewport* getViewport(){return ogreViewport;};

      static Ogre::Camera* getOgreCamera(){return ogreCamera;};

      /*! Push current camera state
       * \note -> it only supports one push/pop (ie: no push-push-pop-pop) */
      static void push();

      /*! Pop camera state previous to pop 
       * @param asTarget true to set as target position, false to set now.
       * \note -> it only supports one push/pop (ie: no push-push-pop-pop) */ 
      static void pop(bool asTarget=false);

      /*! Set phi to a Target value */
      static void setPhi(Ogre::Real value);
   
      /*! Immediate set phi to a value, and clear targets */
      static void setCurrentPhi(Ogre::Real value);
   
      /*! Range value in [-180, 180]. */
      static Ogre::Real rangeValue(Ogre::Real v);

      static Ogre::Real getCenterX(){return(state.center.x);};
      static Ogre::Real getCenterY(){return(state.center.y);};
      static Ogre::Real getCenterZ(){return(state.center.z);};
      static Ogre::Real getTheta(){return(state.theta);};
      static Ogre::Real getPhi(){return(state.phi);};
      static Ogre::Real getZoom(){return(state.zoom);};

      /*! Verify if the camera is at TOP-DOWN view */
      static bool isTopView(){return(state.theta >= 89);};

      /*! Verify if the object under bbox is visible at the current camera
       * \param bbox -> bounding box defining the object 
       * \return -> true if visible, false otherwise */
      static bool isVisible(Ogre::AxisAlignedBox bbox);
   
      /*! Enable camera rotations inputs */
      static void enableRotations();
      /*! Enable camera translations inputs */
      static void enableTranslations();
      /*! Enable zoom changes inputs */
      static void enableZoomChanges();
   
      /*! Disable camera rotations inputs */
      static void disableRotations();
      /*! Disable camera translations inputs */
      static void disableTranslations();
      /*! Disable zoom changes inputs */
      static void disableZoomChanges();
   
      /*! Limit area where camera should be, restricting its position to
       * be inner this area.
       * \note This will affect both positions set by user input or by target.
       * \param min minimun camera values for each axys
       * \param max maximun camera values for each axys */
      static void limitCameraArea(Ogre::Vector3 min, Ogre::Vector3 max);
   
      /*! Disable the limit defined by "limitCameraArea" */
      static void removeCameraAreaLimits();
      
      /*! Set current camera view frustum near clip plane distance.
       * \param nearClip value for near plane distance */
      static void setNearClipDistance(Ogre::Real nearClip);

      /*! Set current camera view frustum far clip plane distance.
       * \param farClip value for far plane distance */
      static void setFarClipDistance(Ogre::Real farClip);

   protected:

      /*! Do the Camera look at */
      static void lookAt();
   
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
      /*! Verify the keyboard input to the Camera
       * \param keboard -> keyboard state */
      static void verifyKeyboardInput();
      /*! Verify mouse input to the Camera
       * \param mouse -> mouse state
       * \return true if Camera moved with mouse */
      static bool verifyMouseInput();
#else
      /*! Verify multitouch input to the Camera */
      static bool verifyMultiTouchInput();
#endif
      /*! Apply a single acceleration
       * \param factor -> factor to apply acceleration to
       * \param ac -> factor acceleration */
      static void applyAcceleration(Ogre::Real& factor, Ogre::Real& ac);
      /*! Apply a single acceleration, verifying the value's limits.
       * When got the limit, the value is limited to it and factor acceleration
       * is automatically set to 0 (stoped).
       * \param factor -> factor to apply acceleration to
       * \param ac -> factor acceleration */
      static void applyAcceleration(Ogre::Real& factor, Ogre::Real& ac,
            Ogre::Real minValue, Ogre::Real maxValue);
      /*! Apply all accelerations and move the Camera (with lookAt)
       * \return true if moved something on Camera, false if remained static */
      static bool applyAccelerationsAndMove();
      /*! Flush current values to the Target current values,
       * usually before setting a Target value */
      static void flushCurrentAtTarget();
   
      /*! Limit a value to its range [min, max].
       * \return value limited to its range. */
      static Ogre::Real limitValue(Ogre::Real v, Ogre::Real min, 
            Ogre::Real max);
   
      static Ogre::Camera* ogreCamera; /**< Pointer to the ogre Camera used */
      static Ogre::Viewport* ogreViewport; /**< pointer to the ogre viewport */

      static CameraState state;       /**< Current camera state */
      static CameraState prevState;   /**< State to push/pop */

      static Ogre::Real phiAc;   /**< Acceleration to side rotation */
      static Ogre::Real thetaAc; /**< Acceleration to up/down rotation */
      static Ogre::Real zoomAc;  /**< Acceleration to zoom value */

      static Kobold::Target targetPhi;     /**< Target value to phi */
      static Kobold::Target targetTheta;   /**< Target value for theta */
      static Kobold::Target targetZoom;    /**< Target value for zoom */
      static Kobold::Target targetX;       /**< Target value for centerX */
      static Kobold::Target targetY;       /**< Target value for centerY */
      static Kobold::Target targetZ;       /**< Target value for centerZ */
   
      static bool canTranslate;    /**< If translations are enabled */
      static bool canRotate;       /**< If rotations are enabled */
      static bool canZoom;         /**< If zoom changes are enabled */

      static bool needUpdate;      /**< If need update to some Target value */

      static Ogre::Vector3 eye;    /**< Eye position */
      
      static Ogre::Real centerXAc;         /**< Center X acceleration */
      static Ogre::Real centerYAc;         /**< Center Y acceleration */
      static Ogre::Real centerZAc;         /**< Center Z acceleration */
   
      static Ogre::Real initialDistance;/**< Initial Distance (used for zoom) */
   
      static bool limitedArea; /**< If camera valid positions area is defined */
      static Ogre::Vector3 minArea; /**< Minimun valid camera positions */
      static Ogre::Vector3 maxArea; /**< Maximun valid camera positions */
      /*! Factor to multiply to screen variation when translating the camera,
       * for multitouch devices. This one is relative to screen size (in big 
       * screens we should use a bigger factor, otherwise the movement will
       * be too slow). */
      static Ogre::Real translateFactor; 
      /*! Max finger distance difference to initial to do a translation 
       * instead of a zoom in/out. As #translateFactor, it's also dependent
       * to the screen resolution. */
      static Ogre::Real maxDistToTranslate;

   private:
      /*! No instances are allowed. */ 
      Camera();
};

}

#endif

