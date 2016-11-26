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

using namespace Goblin;

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
   model = ogreSceneManager->createEntity(modelName, modelFile);

   if(parent)
   {
      node = parent->node->createChildSceneNode();
   }
   else
   {
      node = ogreSceneManager->getRootSceneNode()->createChildSceneNode();
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

   return(res);
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
   return(visible);
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


