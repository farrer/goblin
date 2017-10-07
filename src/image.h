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

#ifndef _goblin_Image_h
#define _goblin_Image_h


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

#include "guiobject.h"

namespace Goblin
{

/*! An Image to be seen at an ogre overlay 
 * \note deprecated: Should use Farso instead.*/
class Image: public GuiObject
{
   public:
      /*! Constructor
       * \param ogreOverlay -> overlay used to put Image in
       * \param imageFileName -> filename of the Image
       * \param groupName -> resource group name where the image is. */
      Image(Ogre::Overlay* ogreOverlay, const Ogre::String& imageFileName, 
            const Ogre::String& groupName);
   
      /*! Constructor for derived classes.
       * \param guiObjectType type of the derived class.
       * \param ogreOverlay -> overlay used to put Image in
       * \param imageFileName -> image's filename
       * \param groupName -> resource group name where the image is. */
      Image(const GuiObjectType& guiObjectType, Ogre::Overlay* ogreOverlay,
            const Ogre::String& imageFileName,
            const Ogre::String& groupName);

      /*! Destructor */
      ~Image();

      /*! Fade the image in 
       * \note: this function will set the image to visible if it is invisible
       *        at the beggining of the fade-in effect. */
      void fadeIn();

      /*! Fade the image out
       * \note: this function will set the image to not visible at end
       *        of fadeout effect */
      void fadeOut();

      /*! \return image file name */
      const Ogre::String& getFileName() const;

   protected:
      /*! Load an Image to a ogre overlay, using defined material */
      void load(const Ogre::String& groupName);

      /*! The update process */
      void _update();
   
      /*! Initialize structures. */
      void init(const Ogre::String& imageFileName, 
            const Ogre::String& groupName);

      Ogre::String imageFile; /**< File name of the Image */
      Ogre::Real alpha;       /**< Current alpha value (on fade-in/out) */
      Ogre::Real deltaAlpha;  /**< Delta alpha value */

      Ogre::MaterialPtr material;        /**< Material used */
      Ogre::TexturePtr texture;          /**< Texture used */
      Ogre::TextureUnitState* texState;  /**< Texture Unit State */

};

}

#endif

#endif

