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
      /*! Constructor */
      Model3d(Ogre::String modelName, Ogre::String modelFile,
            Ogre::SceneManager* sceneManager, Ogre::SceneNode* parentNode=NULL);
      /*! Destructor */
      ~Model3d();

      /*! Change the model material */
      void setMaterial(Ogre::String materialName);

      /*! Set current orientation */
      void setOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);
      /*! Set next orientation */
      void setTargetOrientation(Ogre::Real pitchValue, Ogre::Real yawValue, 
            Ogre::Real rollValue);

      /* Set model's next position */
      void setTargetPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      /*! Set model's current position */
      void setPosition(Ogre::Real pX, Ogre::Real pY, Ogre::Real pZ);
      void setPosition(Ogre::Vector3 p);

      /* Get current model position */
      Ogre::Vector3 getPosition();
      /*! \return Ogre::SceneNode related to the model */
      Ogre::SceneNode* getSceneNode() { return node;};

      /*! Set current model scale */
      void setScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);
      /*! Set next model scale */
      void setTargetScale(Ogre::Real x, Ogre::Real y, Ogre::Real z);

      /*! Update model's position */
      void update();

      /*! hide model */
      void hide();
      /*! show model */
      void show();
      /*! Verify if is visible or not */
      bool isVisible();

   protected:
      Ogre::SceneManager* ogreSceneManager;  /**< Scene manager in use */

      Ogre::SceneNode* node;    /**< Scene Node */
      Ogre::Entity* model;      /**< Model */

      Kobold::Target pos[3];    /**< Target position for model */
      Kobold::Target ori[3];    /**< Orientation angles */
      Kobold::Target scala[3];  /**< Target scale for model */

      bool visible;             /**< If is visible or not */
};


}

#endif

