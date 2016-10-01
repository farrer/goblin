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

#include "screeninfo.h"

using namespace Goblin;

/***********************************************************************
 *                               init                                  *
 ***********************************************************************/
void ScreenInfo::init(Ogre::RenderWindow* ogreRenderWindow,
          Ogre::Real windowScaleFactor, Ogre::uint32 widthThreshold)
{
   /* Retrieve direct access attributes */
   wY = ogreRenderWindow->getHeight();
   wX = ogreRenderWindow->getWidth();
   wScale = windowScaleFactor;
   
   /* Do the simple and often used calculations to screen */
   halfWX = wX / 2;
   halfWY = wY / 2;
   quarterWX = wX / 4;
   quarterWY = wY / 4;
   
   /* Check if should used double sized GUI */
   useDoubleSizedGui = (wX >= widthThreshold);
   guiScale = (useDoubleSizedGui)?2.0f:1.0f;
}

/***********************************************************************
 *                          getWindowWidth                             *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getWindowWidth()
{
   return wX;
}

/***********************************************************************
 *                          getWindowHeight                            *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getWindowHeight()
{
   return wY;
}

/***********************************************************************
 *                        getHalfWindowWidth                           *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getHalfWindowWidth()
{
   return halfWX;
}

/***********************************************************************
 *                        getHalfWindowHeight                          *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getHalfWindowHeight()
{
   return halfWY;
}

/***********************************************************************
 *                      getQuarterWindowWidth                          *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getQuarterWindowWidth()
{
   return quarterWX;
}

/***********************************************************************
 *                       getQuarterWindowHeight                        *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::getQuarterWindowHeight()
{
   return quarterWY;
}


/***********************************************************************
 *                          getWindowHeight                            *
 ***********************************************************************/
Ogre::Real ScreenInfo::getWindowScaleFactor()
{
   return wScale;
}

/***********************************************************************
 *                          getWindowHeight                            *
 ***********************************************************************/
bool ScreenInfo::shouldUseDoubleSizedGui()
{
   return useDoubleSizedGui;
}

/***********************************************************************
 *                            getGuiScale                              *
 ***********************************************************************/
Ogre::Real ScreenInfo::getGuiScale()
{
   return guiScale;
}


/***********************************************************************
 *                        static attributes                            *
 ***********************************************************************/
Ogre::uint32 ScreenInfo::wX;
Ogre::uint32 ScreenInfo::wY;
Ogre::uint32 ScreenInfo::halfWX;
Ogre::uint32 ScreenInfo::halfWY;
Ogre::uint32 ScreenInfo::quarterWX;
Ogre::uint32 ScreenInfo::quarterWY;
Ogre::Real ScreenInfo::wScale;
bool ScreenInfo::useDoubleSizedGui;
Ogre::Real ScreenInfo::guiScale;
