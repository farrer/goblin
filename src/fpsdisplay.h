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

#ifndef _goblin_fps_display_h
#define _goblin_fps_display_h

#include <OGRE/OgreRenderWindow.h>

#include <kobold/timer.h>
#include "textbox.h"

namespace Goblin
{

/*! The FPS display class shows the FPS (Average or Current)
 * on the screen */
class FpsDisplay: public TextBox
{
   public:

      enum FpsDysplayType
      {
         TYPE_AVERAGE,
         TYPE_WORST,
         TYPE_BEST,
         TYPE_LAST
      };

      /*! Constructor */
      FpsDisplay(Ogre::Overlay* ogreOverlay, Ogre::RenderWindow* renderWindow, 
            Ogre::String fontName);
      /*! Destructor */
      ~FpsDisplay();

      /*! Set FpsDisplay Type */
      void setType(int t);

      /*! Update the FpsDisplay */
      void update();

   private:
      /*! update the text with current values */
      void updateText();

      Ogre::RenderWindow* ogreWindow; /**< Current Ogre Window */
      Kobold::Timer lastUpdated;      /**< last updated type */
      int type;

};


}


#endif

