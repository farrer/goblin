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

#ifndef _goblin_model_3d_h
#define _goblin_model_3d_h

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include <kobold/target.h>

namespace Goblin
{

/*! A 3d model abstraction */
class Model3d
{
   public:
      /*! Constructor 
       * \param modelName model's name (unique)
       * \param modelFile filename of model's to load
       * \param sceneManager pointer to Ogre's used SceneManager
       * \param parent pointer to model's parent, if any. */
      Model3d(Ogre::String modelName, Ogre::String modelFile,
            Ogre::SceneManager* sceneManager, Model3d* parent=NULL);
      /*! Destructor */
      virtual ~Model3d();

      /*! Change the model material */
      void setMaterial(Ogre::String materialName);

      /*! Set current orientation along Y axys.
       * \param yawValue new value for Y orientation.
       * \note this function won't change pitch and roll */
      void setOrientation(Ogre::Real yawValue);
      /*! Set current orientation */
      void setOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);
      /*! Set next orientation */
      void setTargetOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);

      /*! \return current model's yaw (Y) angle */
      Ogre::Real getYaw();
      /*! Same as #getYaw()  */
      Ogre::Real getOrientation();

      /* Set model's next position */
      void setTargetPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      /*! Set model's current position */
      void setPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      void setPosition(Ogre::Vector3 p);

      /* Get current model position */
      Ogre::Vector3 getPosition();

      /*! Set current model scale */
      void setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);
      /*! Set next model scale */
      void setTargetScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);

      /*! Update model's position */
      virtual void update();

      /*! hide model */
      void hide();
      /*! show model */
      void show();
      /*! Verify if is visible or not */
      bool isVisible();

      /*! Check if the SceneNode is owned by the model or not.
       * \param node SceneNode to check.
       * \return if node is the SceneNode of this Model or not */
      bool ownSceneNode(Ogre::SceneNode* node);

   protected:
      Ogre::SceneManager* ogreSceneManager;  /**< Scene manager in use */

      Ogre::SceneNode* node;    /**< Scene Node */
      Ogre::Entity* model;      /**< Model */

      Kobold::Target pos[3];    /**< Target position for model */
      Kobold::Target ori[3];    /**< Orientation angles */
      Kobold::Target scala[3];  /**< Target scale for model */

      bool visible;             /**< If is visible or not */
};

/*! A 3d model with animations. This kind must have an Ogre::Skeleton attached
 * to it with some animations.
 * \note AnimatedModel3d must have its 'Idle' animation as the one at first 
 *       index (0). */
class AnimatedModel3d : public Model3d
{
   public:
      /*! Constructor 
       * \param modelName model's name (unique)
       * \param modelFile filename of model's to load
       * \param sceneManager pointer to Ogre's used SceneManager
       * \param totalAnimations total animations supported by the model
       *        (usually it's a value common to all AnimatedModels of some
       *        kind in a game, thus received here as parameter).
       * \param parent pointer to model's parent, if any. */
      AnimatedModel3d(Ogre::String modelName, Ogre::String modelFile,
            Ogre::SceneManager* sceneManager, int totalAnimations,
            Model3d* parent=NULL);
      /*! Destructor */
      virtual ~AnimatedModel3d();

      /*! Update model's position and animations. */
      virtual void update();

      /*! Set model's base animation
       * \param index index of model's new base animation
       * \param loop if the animation should loop at its end.
       * \param reset if will reset animation to its '0' time position 
       * \note to set to no animations, just call it with an index < 0. */
      void setBaseAnimation(int index, bool loop, bool reset = false);

      /*! \return current base animation */
      int getCurrentAnimation();

   private:
      /*! Animation information for AnimatedModel3d animation mixer */
      class AnimationInfo
      {
         public:
            /*! Constructor */
            AnimationInfo();
            /*! Destructor */
            ~AnimationInfo();

            /*! Set related Ogre::AnimationState pointer */
            void setAnimationState(Ogre::AnimationState* animationState);

            /*! \return Ogre::AnimationState related to this animation */
            Ogre::AnimationState* getAnimationState();

            /*! \return if animation is fading in */
            bool isFadingIn();
            /*! \return if animation is fading out */
            bool isFadingOut();
            /*! Set if animation is fading in */
            void setFadingIn(bool value);
            /*! Set if animation is fading out */
            void setFadingOut(bool value);

         private:
            Ogre::AnimationState* animationState;
            bool fadingIn;
            bool fadingOut;
      };

      /*! Update fade-in and fade-out to animation states */
      void doFadeInAndFadeOut();

      int totalAnimations; /**< Total number of animations */
      
      int baseAnimationIndex; /**< Current base animation index */
      AnimationInfo* baseAnimation;    /**< Current base animation used */
      bool looping; /**< If current animation is a lopping one */

      int previousAnimationIndex; /**< Index of the previous looping 
                                    animation (one that we should return to
                                    when the current isn't a looping one) */
      int totalFadings; /**< Total fading-in or fading-out active */

      AnimationInfo* animations; /**< Model animations */
      Ogre::Real timer; /**< Timer reference for our animations */
};

}

#endif

