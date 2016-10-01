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

#include "texttitle.h"

namespace Goblin
{

enum
{
   TEXT_TITLE_STATE_APPEARING,
   TEXT_TITLE_STATE_WAITING,
   TEXT_TITLE_STATE_DISAPPEARING,
   TEXT_TITLE_STATE_DONE
};

#define TEXT_TITLE_WAIT_TIME   1000   /**< 2s */
   
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   #define TEXT_TITLE_SCALE_DELTA   0.03f;
#else
   #define TEXT_TITLE_SCALE_DELTA   0.05f;
#endif
   
/***********************************************************************
 *                            Constructor                              *
 ***********************************************************************/
TextTitle::TextTitle(int x, int y, int w, int h, Ogre::String text, 
            Ogre::String name, Ogre::String fontName)
{
   textOverlay = Ogre::OverlayManager::getSingletonPtr()->create(name + 
      Ogre::String("Overlay"));
   textOverlay->setZOrder(643);
   textOverlay->show();

   dtext = new TextBox(x, y, w, h, text, name, textOverlay, fontName, 16);

   state = TEXT_TITLE_STATE_APPEARING;
   scale = 0.05f;
   textOverlay->setScale(scale, scale);
}

/***********************************************************************
 *                             Destructor                              *
 ***********************************************************************/
TextTitle::~TextTitle()
{
   delete(dtext);
   Ogre::OverlayManager::getSingletonPtr()->destroy(textOverlay);
}

/***********************************************************************
 *                               update                                *
 ***********************************************************************/
void TextTitle::update()
{
   switch(state)
   {
      case TEXT_TITLE_STATE_APPEARING:
      {
         scale += TEXT_TITLE_SCALE_DELTA;
         if(scale > 1)
         {
            /* Done appearing */
            scale = 1;
            state = TEXT_TITLE_STATE_WAITING;

            waitTimer.reset();
         }
         /* Set the scale */
         textOverlay->setScale(scale, scale);
      }
      break;
      case TEXT_TITLE_STATE_WAITING:
      {
         if(waitTimer.getMilliseconds() > TEXT_TITLE_WAIT_TIME)
         {
            state = TEXT_TITLE_STATE_DISAPPEARING;
         }
      }
      break;
      case TEXT_TITLE_STATE_DISAPPEARING:
      {
         scale -= TEXT_TITLE_SCALE_DELTA;
         if(scale < 0.1)
         {
            /* Done disappearing */
            scale = 0.1;
            state = TEXT_TITLE_STATE_DONE;
         }
         /* Set the scale */
         textOverlay->setScale(scale, scale);
      }
      break;
   }
}

/***********************************************************************
 *                               isDone                                *
 ***********************************************************************/
bool TextTitle::isDone()
{
   return(state == TEXT_TITLE_STATE_DONE);
}

}
