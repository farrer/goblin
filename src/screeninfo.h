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

#ifndef _goblin_screen_info_h_
#define _goblin_screen_info_h_

#include <OGRE/OgrePrerequisites.h>

#if (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2)
   #include <OGRE/OgreWindow.h>
#else
   #include <OGRE/OgreRenderWindow.h>
#endif

namespace Goblin
{

/*! Side class strutcture to keep some usefull informations about the current
 * screen used (ie: used as a central location to keep and access them). */
class ScreenInfo
{
   public:
      /*! Init the Screen informations.
       * @param ogreRenderWindow current used Ogre Window. 
       * @param windowScaleFactor Scale factor of the screen (usually, only 
       *        > 1.0 on "retina display" devices). 
       * @param widthThreshold minimun width to use double sized GUI. */
#if (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR >= 2)
      static void init(Ogre::Window* ogreRenderWindow,
                Ogre::Real windowScaleFactor, Ogre::uint32 widthThreshold);
#else
      static void init(Ogre::RenderWindow* ogreRenderWindow,
                Ogre::Real windowScaleFactor, Ogre::uint32 widthThreshold);
#endif
   
      /*! @return current window Width. */
      static Ogre::uint32 getWindowWidth();
      /*! @return current window Height. */
      static Ogre::uint32 getWindowHeight();
   
      /*! @return current window width / 2. */
      static Ogre::uint32 getHalfWindowWidth();
      /*! @return current window height / 2. */
      static Ogre::uint32 getHalfWindowHeight();
   
      /*! @return current window width / 4. */
      static Ogre::uint32 getQuarterWindowWidth();
      /*! @return current window height / 4. */
      static Ogre::uint32 getQuarterWindowHeight();
   
      /*!@return current scale to apply at gui elements */
      static Ogre::Real getGuiScale();
   
      /*! @return current scale factor. Usually > 1.0 in "retina"
       * display devices */
      static Ogre::Real getWindowScaleFactor();
   
      /*! @return if should use double sized GUI or normal one. */
      static bool shouldUseDoubleSizedGui();
   
   
   private:
      /*! Avoid instances. */
      ScreenInfo(){};
   
      static Ogre::uint32 wX; /**< Window Width */
      static Ogre::uint32 wY; /**< Window Height */
   
      static Ogre::uint32 halfWX; /**< Window Width / 2 */
      static Ogre::uint32 halfWY; /**< Window Height / 2 */
   
      static Ogre::uint32 quarterWX; /**< Window Width / 4 */
      static Ogre::uint32 quarterWY; /**< Window Height / 4 */
   
      static Ogre::Real wScale; /**< Screen scale. Usually > 1.0 in "retina"
                                     display devices. */
   
      static bool useDoubleSizedGui; /**< If should use double sized Gui */
      static Ogre::Real guiScale; /**< scale factor to apply to gui elements */
};
   
}


#endif
