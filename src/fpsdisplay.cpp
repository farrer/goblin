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

#include "fpsdisplay.h"
using namespace Goblin;

#define FPS_DISPLAY_UPDATE_TIME  1000 /* 1s */

/***********************************************************************
 *                              constructor                            *
 ***********************************************************************/
FpsDisplay::FpsDisplay(Ogre::Overlay* ogreOverlay, 
                       Ogre::RenderWindow* renderWindow, 
                       Ogre::String fontName)
           :TextBox(0, 0, 64, 32, "", "FpsDisplay", ogreOverlay, fontName, 18)
{
   ogreWindow = renderWindow;
   setText(Ogre::StringConverter::toString(ogreWindow->getAverageFPS()));
   type = TYPE_AVERAGE;
   lastUpdated.reset();
}

/***********************************************************************
 *                              ~destructor                            *
 ***********************************************************************/
FpsDisplay::~FpsDisplay()
{
}

/***********************************************************************
 *                                setType                              *
 ***********************************************************************/
void FpsDisplay::setType(int t)
{
   type = t;
}

/***********************************************************************
 *                                update                               *
 ***********************************************************************/
void FpsDisplay::update()
{
   TextBox::update();
   if( (lastUpdated.getMilliseconds() < FPS_DISPLAY_UPDATE_TIME) &&
       (type != TYPE_LAST) )
   {
      /* No need to update yet */
      return;
   }
   lastUpdated.reset();

   Ogre::String fps;
   switch(type)
   {
      default:
      case TYPE_AVERAGE:
      {
         fps = Ogre::StringConverter::toString(ogreWindow->getAverageFPS(), 4);
      }
      break;
      case TYPE_WORST:
      {
         fps = Ogre::StringConverter::toString(ogreWindow->getWorstFPS(), 4);
      }
      break;
      case TYPE_BEST:
      {
         fps = Ogre::StringConverter::toString(ogreWindow->getBestFPS(), 4);
      }
      break;
      case TYPE_LAST:
      {
         fps = Ogre::StringConverter::toString(ogreWindow->getLastFPS(), 4);
      }
      break;
   }

   fps += Ogre::String(" ") + 
          Ogre::StringConverter::toString(ogreWindow->getTriangleCount());
   fps += Ogre::String(" (") + 
          Ogre::StringConverter::toString(ogreWindow->getBatchCount()) +
          Ogre::String(")");

   setText(fps);
}

