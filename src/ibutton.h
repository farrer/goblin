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

#ifndef _ibutton_h
#define _ibutton_h

#include "image.h"
#include "textbox.h"

namespace Goblin
{

#define IBUTTON_EVENT_NONE         0
#define IBUTTON_EVENT_MOUSE_OVER   1
#define IBUTTON_EVENT_ON_PRESS     2
#define IBUTTON_EVENT_PRESSED      3

/*! The Ibutton class is an Image used as a selectable button. */
class Ibutton: public Image
{
   public:
      
      /*! Constructor
       * \param ogreOverlay -> overlay used to put Image in
       * \param imageFileNam -> filename of the image */
      Ibutton(Ogre::Overlay* ogreOverlay, Ogre::String imageFileName);
   
      /*! Constructor
      * \param ogreOverlay -> overlay used to put Image in
      * \param imageFileNam -> filename of the image */
      Ibutton(Ogre::Overlay* ogreOverlay, Ogre::String imageFileName,
              Ogre::String text, Ogre::String fontName, Ogre::Real charHeight);

      /*! Destructor */
      ~Ibutton();
   
      /*! Set a sound file to play when button is pressed.
       * \param soundFile sound file name */
      void setPressedSound(Ogre::String soundFile);
 
      /*! Verify events on the Ibutton
       * \param mouseX -> cursor X coordinate
       * \param mouseY -> cursor Y coordinate 
       * \param leftButtonPressed -> cursor selected
       * \return IBUTTON event constant */
      int verifyEvents(int mouseX, int mouseY, bool leftButtonPressed);

      /*! Set Image dimensions when mouse over it */
      void setMouseOverDimensions(Ogre::Real w, Ogre::Real h);

      /*! Set Image default dimensions */
      void setDimensions(Ogre::Real w, Ogre::Real h);
   
      /*! Set button text.
       * \note only if it already contains a text 
       * \param str new button text. */
      void setText(Ogre::String str);

      /*! \return current button text (if any).  */
      Ogre::String getText();

   protected:
      /*! Set default values */
      void setDefaults();
   
      Ogre::Real overWidth;   /**< Width when mouse over */
      Ogre::Real overHeight;  /**< Height when mouse over */

      Ogre::Real defaultWidth;   /**< Width when no event */
      Ogre::Real defaultHeight;  /**< Height when no event */
   
      Ogre::String pressedSound; /**< Sound to play for pressed event */
      bool havePressedSound;     /**< If have pressedSound or not */
   
      TextBox* text;          /**< Text box related */

      bool wasPressed;        /**< To Controll pressing events  */
};

}


#endif

