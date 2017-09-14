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

#include <OGRE/OgrePrerequisites.h>

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   #include <OGRE/OgreEntity.h>
#else
   #include <OGRE/OgreItem.h>
   #include <OGRE/Animation/OgreSkeletonAnimation.h>
#endif
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include <kobold/target.h>

#include "goblinconfig.h"

namespace Goblin
{

/*! A 3d model abstraction */
class Model3d
{
   public:

      enum Model3dType
      {
         MODEL_STATIC = 0,
         MODEL_DYNAMIC
      };

      /*! Constructor, with direct load. 
       * \param modelName model's name (unique)
       * \param modelFile filename of model's to load
       * \param groupName resource group where the model is
       * \param sceneManager pointer to Ogre's used SceneManager
       * \param type model type (static or dynamic)
       * \param parent pointer to model's parent, if any.
       * \note on STATIC objects, one must call notifyDirty()
       *       to apply position/orientation changes.
       * \note calling update (and thus using targets for static models
       *       is forbidden). */
      Model3d(const Ogre::String& modelName, const Ogre::String& modelFile,
              const Ogre::String& groupName, Ogre::SceneManager* sceneManager, 
              Model3dType type, Model3d* parent=NULL);

      /*! Constructor, without load.
       * \note must call load later, before using the model */
      Model3d();

      /*! Destructor */
      virtual ~Model3d();

      /*! Load a model.
       * \note only use it on not yet loaded models.
       * \note see full constructor for parameters list. */
      bool load(const Ogre::String& modelName, const Ogre::String& modelFile,
                const Ogre::String& groupName, Ogre::SceneManager* sceneManager,
                Model3dType type, Model3d* parent=NULL);

      /*! Change the model material */
      void setMaterial(const Ogre::String& materialName);

      /*! Set current orientation along Y axys.
       * \param yawValue new value for Y orientation.
       * \note this function won't change pitch and roll */
      void setOrientation(Ogre::Real yawValue);
      /*! Set current orientation */
      void setOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);
      /*! Set next orientation. 
       * \note Will rotate at the nearest equivalent angles. */
      void setTargetOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue, int nSteps = TARGET_DEFAULT_STEPS);
 
      /*! Clear current model orientation, reseting its angles to 0.  */
      void clearOrientation();

      /*! \return current model's pitch (X) angle */
      const Ogre::Real getPitch() const { return ori[0].getValue(); };
      /*! \return current model's yaw (Y) angle */
      const Ogre::Real getYaw() const { return ori[1].getValue(); };
      /*! Same as #getYaw()  */
      const Ogre::Real getOrientation() const { return getYaw(); };
      /*! \return current model's roll (Z) angle */
      const Ogre::Real getRoll() const { return ori[2].getValue(); };

      /* Set model's next position */
      void setTargetPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ,
            int nSteps = TARGET_DEFAULT_STEPS);
      /*! Set model's position */
      void setPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      void setPosition(const Ogre::Vector3& p);

      /* \return current model position */
      const Ogre::Vector3 getPosition() const;

      /*! Set current model scale */
      void setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);
      /*! Set next model scale */
      void setTargetScale(Ogre::Real x, Ogre::Real y, Ogre::Real z,
            int nSteps = TARGET_DEFAULT_STEPS);

      /*! \return current model scale */
      const Ogre::Vector3 getScale() const;

      /*! Set the model's orientation now */
      void setOrientationNow(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);
      /*! Set the model's position now */
      void setPositionNow(const Ogre::Vector3& p);
      void setPositionNow(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      /*! Set the model's scale now */
      void setScaleNow(Ogre::Real x, Ogre::Real y, Ogre::Real z);

      /*! Notify that a static model had its position or 
       * orientation changed. 
       * \note should only be called for STATIC models.
       * \note read Ogre 2.x manual for why to try to call on the same 
       *       frame.*/
      void notifyStaticDirty();

      /*! Update model's position, scale or orientation, according to its
       * defined targets.
       * \return true if any of these elements are updated, false if no
       *         update was needed. */
      virtual bool update();

      /*! hide model */
      void hide();
      /*! show model */
      void show();
      /*! Verify if is visible or not */
      const bool isVisible() const { return visible; };

      /*! \return if the model is static or dynamic 
       * \note only makes sense when using Ogre 2.x */
      bool isStatic();

      /*! Check if the SceneNode is owned by the model or not.
       * \param node SceneNode to check.
       * \return if node is the SceneNode of this Model or not */
      bool ownSceneNode(Ogre::SceneNode* node);

      /*! \return scene node used by the model */
      Ogre::SceneNode* getSceneNode() { return node; };

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
      /*! \return model's Ogre::Entity pointer */
      Ogre::Entity* getEntity() { return model; };
#else
      /*! \return model's Ogre::Item pointer */
      Ogre::Item* getItem() { return model; };

      /*! Update cached mesh information. This info should be used
       * when doing mesh collision detection.
       * \note: avoid calling it too often, as it is an expensive
       *        call and should 'lock' the GPU, killing performance.
       *        Ideally, one should only call this function once per model. */
      void updateCachedMeshInformation();
      /*! Get the cached mesh buffers. If no cache mesh is at the buffers, 
       * will call #updateCachedMeshInformation to generate them. */
      void getCachedMesh(size_t &vertexCount, Ogre::Vector3* &vertices,
            size_t &indexCount, Ogre::uint32* &indices);
#endif

   protected:

      /*! \return equivalent angle to target that is nearest cur */
      float getNearestEquivalentAngle(float cur, float target);

      Ogre::SceneManager* ogreSceneManager;  /**< Scene manager in use */

      Ogre::SceneNode* node;    /**< Scene Node */

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
      Ogre::Entity* model;  /**< Model's Ogre::Entity */
#else
      Ogre::Item* model;    /**< Model's Ogre::Item */
      
      /* Note: for 2.1 we could keep (when needed) a copy of the model's
       * vertices and indexes, to further allow polygon collision check in
       * them. */
      size_t vertexCount;      /**< Current vertex count */
      Ogre::Vector3* vertices; /**< The cached model vertices */
      size_t indexCount;       /**< Current index count */
      Ogre::uint32* indices;   /**< The cached model index */
#endif

      Kobold::Target pos[3];    /**< Target position for model */
      Kobold::Target ori[3];    /**< Orientation angles */
      Kobold::Target scala[3];  /**< Target scale for model */

      bool dirtyPos;          /**< If should update node position */
      bool dirtyOri;          /**< If should update orientation */
      bool dirtyScale;        /**< If should update scale */
      Ogre::Vector3 prevOri;  /**< Previous orientation */

#if OGRE_VERSION_MAJOR != 1
      Ogre::SceneMemoryMgrTypes sceneType;    /**< Model's scene type */
#endif

      bool visible;        /**< If is visible or not */
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
       * \param animationNames vector with name of each animation
       *        supported by the model. The access order (index) will be 
       *        defined by the index of this vector.
       * \param totalAnimations total animations supported by the model
       *        (usually it's a value common to all AnimatedModels of some
       *        kind in a game, thus received here as parameter).
       * \param parent pointer to model's parent, if any. */
      AnimatedModel3d(const Ogre::String& modelName, 
            const Ogre::String& modelFile, const Ogre::String& groupName,
            Ogre::SceneManager* sceneManager, Ogre::String* animationNames,
            int totalAnimations, Model3d* parent=NULL);
      /*! Destructor */
      virtual ~AnimatedModel3d();

      /*! Update model's position, rotation, scale and animations.
       * \return true if updated its position, scale or rotation. */
      virtual bool update();

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

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
            /*! Set related Ogre::AnimationState pointer */
            void setAnimation(Ogre::AnimationState* animation);
            /*! \return Ogre::AnimationState related to this animation */
            Ogre::AnimationState* getAnimation();
#else 
            /*! Set related Ogre::SkeletonAnimation pointer */
            void setAnimation(Ogre::SkeletonAnimation* animation);
            /*! \return Ogre::SkeletonAnimation related to this animation */
            Ogre::SkeletonAnimation* getAnimation();
#endif

            /*! \return if the animation is elapsed or not */
            bool isElapsed(Ogre::Real curTimer);

            /*! \return current global weight of the animation */
            Ogre::Real getWeight();
            /*! Set current animation global weight */
            void setWeight(Ogre::Real weight);

            /*! Set current frame/time to 0 */
            void reset();

            /*! \return if animation is fading in */
            bool isFadingIn();
            /*! \return if animation is fading out */
            bool isFadingOut();
            /*! Set if animation is fading in */
            void setFadingIn(bool value);
            /*! Set if animation is fading out */
            void setFadingOut(bool value);

         private:
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
            Ogre::AnimationState* animation;
#else
            Ogre::SkeletonAnimation* animation;
#endif
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
      bool animationSet; /**< If animation was set at this frame */
};

}

#endif

