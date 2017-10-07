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

#ifndef _goblin_gui_object_h
#define _goblin_gui_object_h

#include <OGRE/OgrePrerequisites.h>

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

#include <OGRE/OgreString.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTextureManager.h>

#include <kobold/target.h>
#include <kobold/list.h>

namespace Goblin
{
   
class GuiObject;

/*! A child of a gui object. */
class GuiObjectChild : public Kobold::ListElement
{
   public:
      /*! Constructor
       * \param childPtr pointer to the child gui object
       * \param offset position offset relative to the parent
       * \note Deleting childPtr is responsability of caller. */
      GuiObjectChild(GuiObject* childPtr, const Ogre::Vector2& offset);
      /*! Destructor  */
      ~GuiObjectChild();
   
      /* Reset child position based on its parent one. */
      void resetPosition(Ogre::Real parentX, Ogre::Real parentY);
   
      GuiObject* ptr;  /**< Current child gui object pointer */
      Ogre::Vector2 offset; /**< Child offset position */
};

/*! The basic object for gui usage 
 * \note deprecated: Should use Farso instead.*/
class GuiObject : protected Kobold::List
{
   public:
   
      enum GuiObjectType
      {
         TYPE_IMAGE,
         TYPE_TEXT_BOX,
         TYPE_IBUTTON,
         TYPE_TEXT_TITLE,
         TYPE_IBAR
      };
 
      /*! Constructor
       * \ogreOverlay -> overlay to show image in */
      GuiObject(Ogre::Overlay* ogreOverlay, const GuiObjectType& type,
            const Ogre::String& overlayElementType, 
            const Ogre::String& elementName);
      /*! Destructor */
      virtual ~GuiObject();

      /*! Set the GuiObject position
       * \param x -> X coordinate on screen
       * \param y -> Y coordinate on screen */
      void setPosition(Ogre::Real x, Ogre::Real y);
      /*! Set GuiObject dimensions
       * \param w -> width
       * \param h -> height */
      void setDimensions(Ogre::Real w, Ogre::Real h);

      /*! Verify if point (x,y) is inner the GuiObject */
      bool isPointIn(Ogre::Real x, Ogre::Real y);

      /*! Hide the GuiObject */
      void hide();

      /*! Show the GuiObject */
      void show();

      /*! Verify if the GuiObject is visible or not */
      const bool isVisible() const;

      /*! Update the GuiObject
       * @param force true to force update.
       * \note: it's only usefull and needed for GuiObjects
       *       that'll have its size or position changed by
       *       Target values (by setTarget* ) */
      void update(bool force=false);

      /*! Set a Target position and dimension */
      void setTarget(Ogre::Real x, Ogre::Real y, Ogre::Real w, Ogre::Real h,
            int nSteps=TARGET_DEFAULT_STEPS);
      /*! Set a Target dimension */
      void setTargetDimensions(Ogre::Real w, Ogre::Real h,
            int nSteps=TARGET_DEFAULT_STEPS);
      /*! Set a Target position */
      void setTargetPosition(Ogre::Real x, Ogre::Real y,
            int nSteps=TARGET_DEFAULT_STEPS);

      /*! Get current GuiObject width */
      const Ogre::Real getWidth() const;
      /*! Get current GuiObject height */
      const Ogre::Real getHeight() const;
      /*! Get current GuiObject X position */
      const Ogre::Real getPosX() const;
      /*! Get current GuiObject Y position */
      const Ogre::Real getPosY() const;

      /*! Get target GuiObject X position */
      const Ogre::Real getTargetPosX() const;
      /*! Get target GuiObject Y position */
      const Ogre::Real getTargetPosY() const;


      /*! Verify if the GuiObject is currently updating or not */
      bool isUpdating();
   
      /*! Add object as a child
       * \param obj -> child object
       * \param offX -> child offset on X
       * \param offY -> child offset on Y */
      void addChild(GuiObject* obj, Ogre::Real offX, Ogre::Real offY);
   
      /*! Get child GuiObjectChild pointer, based on its GuiObject.
       * \param obj pointer to the GuiObject to find as child of this one
       * \return GuiObjectChild found or null if couldn't find. */
      GuiObjectChild* getChild(GuiObject* obj);

   protected:
      /*! Internal setPosition */
      void setPosition(Ogre::Real x, Ogre::Real y, bool update);
      /*! Internal setDimensions */
      void setDimensions(Ogre::Real w, Ogre::Real h, bool update);
   
      /*! Internal update */
      virtual void _update()=0;

      Kobold::Target tX;              /**< X Target value */
      Kobold::Target tY;              /**< Y Target value */
      Kobold::Target tW;              /**< Width Target value */
      Kobold::Target tH;              /**< Height Target value */
      bool needUpdate;        /**< if need to update targets */

      bool visible;           /**< True if visible */

      Ogre::Overlay* guiOverlay;         /**< Overlay */
      Ogre::OverlayContainer* container; /**< Container to the GuiObject */
   
   private:
   
      int type; /**< The type of the implementer GuiObject class. */
   
      static int internalCounter;    /**< Internal counter used on name */

};

}

#endif

#endif

