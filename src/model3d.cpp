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

#include "model3d.h"

#include <OGRE/OgreSkeleton.h>
#if OGRE_VERSION_MAJOR == 1
   #include <OGRE/OgreSkeletonInstance.h>
#endif

#include <kobold/log.h>

#include <assert.h>

using namespace Goblin;

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                                Model3d                                //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
Model3d::Model3d(Ogre::String modelName, Ogre::String modelFile,
            Ogre::SceneManager* sceneManager, Model3d* parent)
{
   visible = true;

   /* Set the scene manager to use */
   ogreSceneManager = sceneManager;

   /* Create the model and scene node */
#if OGRE_VERSION_MAJOR == 1
   model = ogreSceneManager->createEntity(modelName, modelFile);
#else
   model = ogreSceneManager->createEntity(modelFile);
   if(!model)
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR, 
               "Couldn't create entity '%s' ('%s'), is it a v2 mesh?",
               modelFile.c_str(), modelName.c_str());
   }
   model->setName(modelName);
#endif

   if(parent)
   {
      node = parent->node->createChildSceneNode();
   }
   else
   {
#if OGRE_VERSION_MAJOR == 1
      node = ogreSceneManager->getRootSceneNode()->createChildSceneNode();
#else
      //TODO: set if static or dynamic!
      node = ogreSceneManager->getRootSceneNode()->createChildSceneNode(
            Ogre::SCENE_DYNAMIC);
#endif
   }
   node->attachObject(model);
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
Model3d::~Model3d()
{
   /* Remove model and node */
   node->detachObject(model);
   ogreSceneManager->destroySceneNode(node);
   ogreSceneManager->destroyEntity(model);
}

/***********************************************************************
 *                           changeMaterial                            *
 ***********************************************************************/
void Model3d::setMaterial(Ogre::String materialName)
{
   model->setMaterialName(materialName);
}

/***********************************************************************
 *                           setOrientation                            *
 ***********************************************************************/
void Model3d::setOrientation(Ogre::Real yawValue)
{
   node->yaw(Ogre::Radian(Ogre::Degree(yawValue-ori[1].getValue())));
   ori[1].setCurrent(yawValue);
}

/***********************************************************************
 *                                getYaw                               *
 ***********************************************************************/
Ogre::Real Model3d::getYaw()
{
   return ori[1].getValue();
}
Ogre::Real Model3d::getOrientation()
{
   return getYaw();
}

/***********************************************************************
 *                           setOrientation                            *
 ***********************************************************************/
void Model3d::setOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue)
{
   /* Define scene node, based on previous */
   node->pitch(Ogre::Radian(Ogre::Degree(pitchValue-ori[0].getValue())));
   node->yaw(Ogre::Radian(Ogre::Degree(yawValue-ori[1].getValue())));
   node->roll(Ogre::Radian(Ogre::Degree(rollValue-ori[2].getValue())));

   /* Define Target */
   ori[0].setCurrent(pitchValue);
   ori[1].setCurrent(yawValue);
   ori[2].setCurrent(rollValue);
}

/***********************************************************************
 *                        setTargetOrientation                         *
 ***********************************************************************/
void Model3d::setTargetOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue)
{
   /* Define Target */
   ori[0].setTarget(pitchValue);
   ori[1].setTarget(yawValue);
   ori[2].setTarget(rollValue);
}

/***********************************************************************
 *                             setPosition                             *
 ***********************************************************************/
void Model3d::setPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ)
{
   /* Set Target */
   pos[0].setCurrent(pX);
   pos[1].setCurrent(pY);
   pos[2].setCurrent(pZ);

   /* Set node */
   node->setPosition(pX, pY, pZ);
}

/***********************************************************************
 *                             setPosition                             *
 ***********************************************************************/
void Model3d::setPosition(Ogre::Vector3 p)
{
   setPosition(p.x, p.y, p.z);
}

/***********************************************************************
 *                             getPosition                             *
 ***********************************************************************/
Ogre::Vector3 Model3d::getPosition()
{
   Ogre::Vector3 res;
   res.x = pos[0].getValue();
   res.y = pos[1].getValue();
   res.z = pos[2].getValue();

   return res;
}

/***********************************************************************
 *                          setTargetPosition                          *
 ***********************************************************************/
void Model3d::setTargetPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ)
{
   /* Set Target */
   pos[0].setTarget(pX);
   pos[1].setTarget(pY);
   pos[2].setTarget(pZ);
}

/***********************************************************************
 *                              setScale                               *
 ***********************************************************************/
void Model3d::setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
   /* Set Target */
   scala[0].setCurrent(x);
   scala[1].setCurrent(y);
   scala[2].setCurrent(z);

   /* Set node */
   node->setScale(x, y, z);
}

/***********************************************************************
 *                           setTargetScale                            *
 ***********************************************************************/
void Model3d::setTargetScale(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
   /* Set Target */
   scala[0].setTarget(x);
   scala[1].setTarget(y);
   scala[2].setTarget(z);
}

/***********************************************************************
 *                                hide                                 *
 ***********************************************************************/
void Model3d::hide()
{
   visible = false;
   node->setVisible(false);
}

/***********************************************************************
 *                                show                                 *
 ***********************************************************************/
void Model3d::show()
{
   visible = true;
   node->setVisible(true);
}

/***********************************************************************
 *                             isVisible                               *
 ***********************************************************************/
bool Model3d::isVisible()
{
   return visible;
}

/***********************************************************************
 *                            ownSceneNode                             *
 ***********************************************************************/
bool Model3d::ownSceneNode(Ogre::SceneNode* node)
{
   return this->node == node;
}

/***********************************************************************
 *                               update                                *
 ***********************************************************************/
void Model3d::update()
{
   /* Update position */
   if( (pos[0].needUpdate()) || (pos[1].needUpdate()) || (pos[2].needUpdate()) )
   {
      pos[0].update();
      pos[1].update();
      pos[2].update();
      node->setPosition(pos[0].getValue(),pos[1].getValue(),pos[2].getValue());
   }

   /* Update scale */
   if( (scala[0].needUpdate()) || (scala[1].needUpdate()) || 
       (scala[2].needUpdate()) )
   {
      scala[0].update();
      scala[1].update();
      scala[2].update();
      node->setScale(scala[0].getValue(), scala[1].getValue(),
            scala[2].getValue());
   }

   /* Update node orientation */
   if(ori[0].needUpdate())
   {
      ori[0].update();
      node->pitch(Ogre::Radian(Ogre::Degree(ori[0].getLastDelta())));
   }
   if(ori[1].needUpdate())
   {
      ori[1].update();
      node->yaw(Ogre::Radian(Ogre::Degree(ori[1].getLastDelta())));
   }
   if(ori[2].needUpdate())
   {
      ori[2].update();
      node->roll(Ogre::Radian(Ogre::Degree(ori[2].getLastDelta())));
   }
}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                            AnimationInfo                              //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
AnimatedModel3d::AnimationInfo::AnimationInfo()
{
   animationState = NULL;
   fadingIn = false;
   fadingOut = false;
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
AnimatedModel3d::AnimationInfo::~AnimationInfo()
{
}

/***********************************************************************
 *                         setAnimationState                           *
 ***********************************************************************/
void AnimatedModel3d::AnimationInfo::setAnimationState(
      Ogre::AnimationState* animationState)
{
   this->animationState = animationState;
   this->animationState->setEnabled(false);
}

/***********************************************************************
 *                         getAnimationState                           *
 ***********************************************************************/
Ogre::AnimationState* AnimatedModel3d::AnimationInfo::getAnimationState()
{
   return animationState;
}

/***********************************************************************
 *                             isFadingIn                              *
 ***********************************************************************/
bool AnimatedModel3d::AnimationInfo::isFadingIn()
{
   return fadingIn;
}

/***********************************************************************
 *                            isFadingOut                              *
 ***********************************************************************/
bool AnimatedModel3d::AnimationInfo::isFadingOut()
{
   return fadingOut;
}

/***********************************************************************
 *                            setFadingIn                              *
 ***********************************************************************/
void AnimatedModel3d::AnimationInfo::setFadingIn(bool value)
{
   fadingIn = value;
}

/***********************************************************************
 *                            setFadingOut                             *
 ***********************************************************************/
void AnimatedModel3d::AnimationInfo::setFadingOut(bool value)
{
   fadingOut = value;
}

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                           AnimatedModel3d                             //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


/*! Animation crossfade speed in % of full weight per second */
#define ANIM_FADE_SPEED 7.5f
/*! Animation update rate (in seconds). */
#define ANIM_UPDATE_RATE (BASE_APP_UPDATE_RATE / 1000.0f)

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
AnimatedModel3d::AnimatedModel3d(Ogre::String modelName, 
                      Ogre::String modelFile, Ogre::SceneManager* sceneManager, 
                      int totalAnimations, Model3d* parent)
                :Model3d(modelName, modelFile, sceneManager, parent)
{
   assert(totalAnimations > 0);
   this->totalAnimations = totalAnimations;
   this->baseAnimation = NULL;
   this->animations = new AnimationInfo[totalAnimations];
   this->baseAnimationIndex = -1;
   this->looping = false;
   this->previousAnimationIndex = -1;
   this->timer = 0;
   this->totalFadings = 0;

#if OGRE_VERSION_MAJOR == 1
   /* Define animation blend */
   model->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
#endif

   /* Let's define each animation pointer */
   Ogre::AnimationStateSet* anims = model->getAllAnimationStates();
   int i=0;
   for(Ogre::AnimationStateIterator iter = anims->getAnimationStateIterator();
       iter.hasMoreElements(); iter.moveNext())
   {
      if(i < totalAnimations)
      {
         animations[i].setAnimationState(iter.peekNextValue());
      }
      
      i++;
   }

   /* Check total got */
   if(i != totalAnimations)
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR, 
            "Warning: got %d animations for model '%s' but was expecting %d",
            i, modelFile.c_str(), totalAnimations);
   }
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
AnimatedModel3d::~AnimatedModel3d()
{
   delete[] animations;
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
void AnimatedModel3d::update()
{
   Model3d::update();

   timer += ANIM_UPDATE_RATE;

   /* Check finish non-looping animations */
   if((!looping) && (baseAnimation) && 
      (timer > baseAnimation->getAnimationState()->getLength()))
   {
      /* A non looping animation just finished, let's reset to previous
       * looping one. */
      setBaseAnimation(previousAnimationIndex, true); 
   }

   /* Update base animation time */
   if(baseAnimation)
   {
      baseAnimation->getAnimationState()->addTime(ANIM_UPDATE_RATE);
   }

   assert(totalFadings >= 0);
   if(totalFadings > 0)
   {
      doFadeInAndFadeOut();
   }
}

/***********************************************************************
 *                           doFadeInAndOut                            *
 ***********************************************************************/
void AnimatedModel3d::doFadeInAndFadeOut()
{
   /* let's apply the fade-in - fade-out */
   Ogre::Real newWeight;
   for(int i = 0; i < totalAnimations; i++)
   {
      if(animations[i].isFadingIn())
      {
         /* Increment its weight, untill full. */
         newWeight = animations[i].getAnimationState()->getWeight() +
                  ANIM_UPDATE_RATE * ANIM_FADE_SPEED;
         if(newWeight >= 0.98f)
         {
            /* Done fading-in */
            animations[i].getAnimationState()->setWeight(1.0f);
            animations[i].setFadingIn(false);
            totalFadings--;
         }
         else
         {
            animations[i].getAnimationState()->setWeight(newWeight);
         }
      }
      else if (animations[i].isFadingOut())
      {
         /* Decrement its weight, until irrelevant */
         newWeight = animations[i].getAnimationState()->getWeight() - 
            ANIM_UPDATE_RATE * ANIM_FADE_SPEED;          
         animations[i].getAnimationState()->setWeight(
               Ogre::Math::Clamp<Ogre::Real>(newWeight, 0, 1));
         if (newWeight <= 0)
         {
            /* Done fading out */
            animations[i].getAnimationState()->setEnabled(false);
            animations[i].setFadingOut(false);
            totalFadings--;
         }
      }
   }

}

/***********************************************************************
 *                           getBaseAnimation                          *
 ***********************************************************************/
int AnimatedModel3d::getCurrentAnimation()
{
   return baseAnimationIndex;
}

/***********************************************************************
 *                           setBaseAnimation                          *
 ***********************************************************************/
void AnimatedModel3d::setBaseAnimation(int index, bool loop, bool reset)
{
   if(index == baseAnimationIndex)
   {
      /* Setting to the current animation, no need to do anything. */
      return;
   }

   /* Check if have a previous animation to fade-out */
   if(baseAnimation)
   {
      if(baseAnimation->isFadingIn())
      {
         baseAnimation->setFadingIn(false);
         totalFadings--;
      }
      if(!baseAnimation->isFadingOut())
      {
         baseAnimation->setFadingOut(true);
         totalFadings++;
      }
   }

   /* Update last looping animation, if needed */
   if((!loop) && (looping))
   {
      previousAnimationIndex = baseAnimationIndex;
   }
   looping = loop;

   /* If not looping, we must set our internal timer */
   if(!loop)
   {
      timer = 0;
   }

   /* Define new current index */
   baseAnimationIndex = index;

   /* Define new base animation */
   if((index >= 0) && (index < totalAnimations))
   {
      baseAnimation = &animations[index]; 

      /* Enable it */
      baseAnimation->getAnimationState()->setEnabled(true);
      baseAnimation->getAnimationState()->setLoop(loop);
      baseAnimation->getAnimationState()->setWeight(0.0f);

      /* Check fade-in and fade-out (and its totals) */
      if(baseAnimation->isFadingOut())
      {
         baseAnimation->setFadingOut(false);
         totalFadings--;
      }
      if(!baseAnimation->isFadingIn())
      {
         baseAnimation->setFadingIn(true);
         totalFadings++;
      }
      if(reset)
      {
         baseAnimation->getAnimationState()->setTimePosition(0);
      }
   }
   else
   {
      baseAnimation = NULL;
   }
}

