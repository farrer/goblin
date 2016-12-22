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

#include "guiobject.h"

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

#include "screeninfo.h"

namespace Goblin
{

/***********************************************************************
 *                                Constructor                          *
 ***********************************************************************/
GuiObjectChild::GuiObjectChild(GuiObject* childPtr, Ogre::Vector2 offset)
{
   this->ptr = childPtr;
   this->offset = offset;
}
/***********************************************************************
 *                             Destructor                              *
 ***********************************************************************/
void GuiObjectChild::resetPosition(Ogre::Real parentX, Ogre::Real parentY)
{
   ptr->setPosition(parentX + offset.x, parentY + offset.y);
}

/***********************************************************************
 *                             Destructor                              *
 ***********************************************************************/
GuiObjectChild::~GuiObjectChild()
{
}
   
/***********************************************************************
 *                                Constructor                          *
 ***********************************************************************/
GuiObject::GuiObject(Ogre::Overlay* ogreOverlay, int type,
      Ogre::String overlayElementType, Ogre::String elementName)
   : Kobold::List()
{
   Ogre::String name;
   needUpdate = false;
   visible = true;
   this->type = type;
   
   /* Set an unique name */
   name = elementName + Ogre::StringConverter::toString(internalCounter);

   /* Create the container to the Object */
   container = (Ogre::OverlayContainer*)
      Ogre::OverlayManager::getSingletonPtr()->createOverlayElement(
            overlayElementType, name);

   /* Set it at the overlay */
   guiOverlay = ogreOverlay;
   guiOverlay->add2D(container);
   
   /* Increment counter */
   internalCounter++;
}

/***********************************************************************
 *                             Destructor                              *
 ***********************************************************************/
GuiObject::~GuiObject()
{
   /* Free the children (should be called here, as we'll destroy 
    * the containers before the parent's destructor is called). */
   clearList();

   /* Remove from overlay */
   guiOverlay->remove2D(container);
   Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(container);
}

/***********************************************************************
 *                              addChild                               *
 ***********************************************************************/
void GuiObject::addChild(GuiObject* obj, Ogre::Real offX,
                         Ogre::Real offY)
{
   GuiObjectChild* child = new GuiObjectChild(obj, Ogre::Vector2(offX, offY));
   child->resetPosition(getPosX(), getPosY());
   insert(child);
}

/***********************************************************************
 *                              getChild                               *
 ***********************************************************************/
GuiObjectChild* GuiObject::getChild(GuiObject* obj)
{
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i=0; i < getTotal(); i++)
   {
      if(child->ptr == obj)
      {
         return child;
      }
      child = (GuiObjectChild*)child->getNext();
   }
   
   return NULL;
}

/***********************************************************************
 *                              isPointIn                              *
 ***********************************************************************/
bool GuiObject::isPointIn(Ogre::Real x, Ogre::Real y)
{
   /* Check object container */
   if((type != TYPE_TEXT_BOX) && (type != TYPE_TEXT_TITLE))
   {
      if(container->contains(x / ScreenInfo::getWindowWidth(),
                             y / ScreenInfo::getWindowHeight()))
      {
         /* Is inner, ok. */
         return true;
      }
   }
   else
   {
      if(container->contains(x, y))
      {
         return true;
      }
   }
   
   /* Not inner, must check each child */
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i=0; i < getTotal(); i++)
   {
      if(child->ptr->isPointIn(x, y))
      {
         return true;
      }
      child = (GuiObjectChild*)child->getNext();
   }
   
   return false;
}

/***********************************************************************
 *                              setPosition                            *
 ***********************************************************************/
void GuiObject::setPosition(Ogre::Real x, Ogre::Real y)
{
   setPosition(x, y, true);
}

/***********************************************************************
 *                              setPosition                            *
 ***********************************************************************/
void GuiObject::setPosition(Ogre::Real x, Ogre::Real y, bool update)
{
   if(update)
   {
      tX.setCurrent(x);
      tY.setCurrent(y);
   }
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i = 0; i < getTotal(); i++)
   {
      child->ptr->setPosition(x + child->offset.x,
                              y + child->offset.y, update);
      child = (GuiObjectChild*) child->getNext();
   }
   if((type != TYPE_TEXT_BOX) && (type != TYPE_TEXT_TITLE))
   {
      container->setPosition(x / ScreenInfo::getWindowWidth(),
                             y / ScreenInfo::getWindowHeight());
   }
   else
   {
      container->setPosition(x, y);
   }
}

/***********************************************************************
 *                             setDimensions                           *
 ***********************************************************************/
void GuiObject::setDimensions(Ogre::Real w, Ogre::Real h)
{
   setDimensions(w, h, true);
}

/***********************************************************************
 *                             setDimensions                           *
 ***********************************************************************/
void GuiObject::setDimensions(Ogre::Real w, Ogre::Real h, bool update)
{
   if(update)
   {
      tW.setCurrent(w);
      tH.setCurrent(h);
   }
   if((type != TYPE_TEXT_BOX) && (type != TYPE_TEXT_TITLE))
   {
      container->setDimensions(w / ScreenInfo::getWindowWidth(),
                               h / ScreenInfo::getWindowHeight());
   }
   else
   {
      container->setDimensions(w, h);
   }
}

/***********************************************************************
 *                              setTarget                              *
 ***********************************************************************/
void GuiObject::setTarget(Ogre::Real x, Ogre::Real y, 
      Ogre::Real w, Ogre::Real h, int nSteps)
{
   tX.setTarget(x, nSteps);
   tY.setTarget(y, nSteps);
   tW.setTarget(w, nSteps);
   tH.setTarget(h, nSteps);
   needUpdate = true;
}

/***********************************************************************
 *                         setTargetDimensions                         *
 ***********************************************************************/
void GuiObject::setTargetDimensions(Ogre::Real w, Ogre::Real h, int nSteps)
{
   tW.setTarget(w, nSteps);
   tH.setTarget(h, nSteps);
   needUpdate = true;
}

/***********************************************************************
 *                         setTargetPosiiton                           *
 ***********************************************************************/
void GuiObject::setTargetPosition(Ogre::Real x, Ogre::Real y, int nSteps)
{
   tX.setTarget(x, nSteps);
   tY.setTarget(y, nSteps);
   needUpdate = true;
}

/***********************************************************************
 *                                 update                              *
 ***********************************************************************/
void GuiObject::update(bool force)
{
   if(needUpdate || force)
   {
      /* Update Target variables */
      tX.update();
      tY.update();
      tW.update();
      tH.update();

      /* Set position and dimensions */
      setPosition(tX.getValue(), tY.getValue(), false);
      setDimensions(tW.getValue(), tH.getValue(), false);

      /* Recalculate the flag */
      needUpdate = ( tX.needUpdate() || tY.needUpdate() ||
            tW.needUpdate() || tH.needUpdate() );

      /* Call specific implementation object */
      _update();
      
      /* Call child update */
      GuiObjectChild* child = (GuiObjectChild*)getFirst();
      for(int i=0; i < getTotal(); i++)
      {
         child->ptr->update(force);
         child = (GuiObjectChild*)child->getNext();
      }
   }
}

/***********************************************************************
 *                                 hide                                *
 ***********************************************************************/
void GuiObject::hide()
{
   container->hide();
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i=0; i < getTotal(); i++)
   {
      child->ptr->hide();
      child = (GuiObjectChild*)child->getNext();
   }
   visible = false;
}

/***********************************************************************
 *                                 show                                *
 ***********************************************************************/
void GuiObject::show()
{
   container->show();
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i=0; i < getTotal(); i++)
   {
      child->ptr->show();
      child = (GuiObjectChild*)child->getNext();
   }
   visible = true;
}

/***********************************************************************
 *                              isVisible                              *
 ***********************************************************************/
bool GuiObject::isVisible()
{
   return(visible);
}

/***********************************************************************
 *                               getWidth                              *
 ***********************************************************************/
Ogre::Real GuiObject::getWidth()
{
   return(tW.getValue());
}

/***********************************************************************
 *                               getHeight                             *
 ***********************************************************************/
Ogre::Real GuiObject::getHeight()
{
   return(tH.getValue());
}

/***********************************************************************
 *                               getPosX                               *
 ***********************************************************************/
Ogre::Real GuiObject::getPosX()
{
   return(tX.getValue());
}

/***********************************************************************
 *                               getPosY                               *
 ***********************************************************************/
Ogre::Real GuiObject::getPosY()
{
   return(tY.getValue());
}

/***********************************************************************
 *                               getPosX                               *
 ***********************************************************************/
Ogre::Real GuiObject::getTargetPosX()
{
   return(tX.getTarget());
}

/***********************************************************************
 *                               getPosY                               *
 ***********************************************************************/
Ogre::Real GuiObject::getTargetPosY()
{
   return(tY.getTarget());
}

/***********************************************************************
 *                             isUpdating                              *
 ***********************************************************************/
bool GuiObject::isUpdating()
{
   if(needUpdate)
   {
      return true;
   }
   
   GuiObjectChild* child = (GuiObjectChild*)getFirst();
   for(int i=0; i < getTotal(); i++)
   {
      if(child->ptr->isUpdating())
      {
         return true;
      }
      child = (GuiObjectChild*)child->getNext();
   }
   return false;
}

/***********************************************************************
 *                           static members                            *
 ***********************************************************************/
int GuiObject::internalCounter = 0;

}

#endif

