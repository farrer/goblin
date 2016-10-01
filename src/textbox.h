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

#ifndef _goblin_text_box_h
#define _goblin_text_box_h

#include <OGRE/OgreString.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/Overlay/OgreTextAreaOverlayElement.h>
#include <OGRE/Overlay/OgreFontManager.h>

#include "guiobject.h"

namespace Goblin
{

/*! A box with a text on the 2d screen */
class TextBox : public GuiObject
{
   public:
      /*! Constructor
       * \param x -> x coordinate (if center align: center, else, left)
       * \param y -> y coordinate (left corner or center)
       * \param h -> height
       * \param w -> width
       * \param text -> text of the box
       * \param name -> name of the box
       * \param ogreOverlay -> pointer to the overlay used
       * \param fontName -> name of font to use
       * \param charHeight -> height of character*/
      TextBox(int x, int y, int w, int h, Ogre::String text, 
            Ogre::String name, Ogre::Overlay* ogreOverlay,
            Ogre::String fontName, Ogre::Real charHeight);
      /*! Destructor */
      ~TextBox();

      /*! Set the text
       * \param text -> new text to display */
      void setText(Ogre::String text);
      
      /*! \return current text */
      Ogre::String getText();

      /*! Set the text font
       * \param fontName -> name of the font to use
       * \param size -> font size */
      void setFont(Ogre::String fontName, Ogre::Real size);

      /*! Set color of the text */
      void setColor(Ogre::Real r, Ogre::Real g, Ogre::Real b, Ogre::Real a);

      /*! Set text alignment */
      void setAlignment(Ogre::TextAreaOverlayElement::Alignment a);

      /*! Fade the text in */
      void fadeIn();

      /*! Fade the text out */
      void fadeOut();

   protected:
      /*! The internal update. Not used for TextBox. */
      void _update();

      Ogre::TextAreaOverlayElement* dispText;  /**< text to display */
      Ogre::ColourValue color;  /**< Current color */
      Ogre::Real deltaAlpha;  /**< Delta alpha value */
};

}

#endif

