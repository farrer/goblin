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

#ifndef _goblin_text_title_h
#define _goblin_text_title_h

#include <OGRE/OgrePrerequisites.h>

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)


#include "textbox.h"
#include <kobold/timer.h>

#include <OGRE/OgreString.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>

namespace Goblin
{

/*! The text title is used to show BIG animated texts on screen,
 * usually for level titles and "You Win" messages.
 * \note deprecated: Should use Farso instead.*/
class TextTitle
{
   public:
      /*! Constructor
       * \param x -> x coordinate
       * \param y -> y coordinate
       * \param h -> height
       * \param w -> width
       * \param text -> text of the box
       * \param name -> name of the box
       * \param fontName -> name of the font to use */
      TextTitle(int x, int y, int w, int h, Ogre::String text, 
            Ogre::String name, Ogre::String fontName);
      /*! Destructor */
      ~TextTitle();

      /*! Update the TextTitle animation */
      void update();

      /*! Verify if text animation is done for some time 
       * \return true when first animation cicle is done */
      bool isDone();

   protected:
      Ogre::Overlay* textOverlay;    /**< The Overlay Used */
      Goblin::TextBox* dtext;        /**< The Text itself */

      int state;                     /**< Current state */
      Ogre::Real scale;              /**< Current scale factor */
      Kobold::Timer waitTimer;       /**< Timer to wait */
};

}

#endif

#endif

