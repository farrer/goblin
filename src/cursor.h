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

#ifndef _goblin_cursor_h
#define _goblin_cursor_h

#include <kobold/platform.h>

#if KOBOLD_PLATFORM != KOBOLD_PLATFORM_IOS &&\
    KOBOLD_PLATFORM != KOBOLD_PLATFORM_ANDROID

#include <kobold/mouse.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlayManager.h>
#include <OGRE/Overlay/OgreOverlayContainer.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreRenderWindow.h>

namespace Goblin
{

/*! A mouse Cursor class.*/
class Cursor
{
   public:
      /*! Cursor Initialize
       * \ogreWindow -> pointer to the used window */
      static void init(Ogre::RenderWindow* ogreWindow);
      /*! Finish the use of mouse cursor*/
      static void finish();  

      /*! Set the current mouse Cursor to an image
       * \param fileName -> name of the image to load */
      static void set(Ogre::String fileName);

      /*! Update the Cursor position and state 
       * \note: should be called after SDL_PumEvents or similar. */
      static void update();

      /*! Update current mouse by events. Usually used in conjunction
       * with #update */
      static void updateByEvent(SDL_Event event);

      /*! \return current cursor X coordinate on screen */
      static int getX();
      /*! \return current cursor Y coordinate on screen */
      static int getY();
      /*! \return cursor relative to last X coordinate movement  */
      static int getRelativeX();
      /*! \return cursor relative to last Y coordinate movement  */
      static int getRelativeY();
      /*! \return current cursor wheel relative state from last check */
      static int getRelativeWheel();

      /*! Check if left button is pressed */
      static bool isLeftButtonPressed();
      /*! Check if middle button is pressed */
      static bool isMiddleButtonPressed();
      /*! Check if right button is pressed */
      static bool isRightButtonPressed();
      /*! \return if mouse button released on last check. */
      static bool checkButtonRelease(int buttonNumber);


   private:
      static Ogre::Overlay* ogreGuiOverlay;
      static Ogre::OverlayContainer* ogreCursorContainer;
      static Ogre::TexturePtr cursorTexture;
      static Ogre::MaterialPtr cursorMaterial;
      
      static Ogre::Real windowX;
      static Ogre::Real windowY;

      /*! Load Cursor file 
       * \param fileName -> file name of Cursor */
      static void loadCursor(Ogre::String fileName);
};

}

#endif

#endif

