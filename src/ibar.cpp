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

#include "ibar.h"
#include <OGRE/OgreMath.h>

using namespace Goblin;

/***********************************************************************
 *                           Constructor                               *
 ***********************************************************************/
Ibar::Ibar(Ogre::Overlay* ogreOverlay, Ogre::String fileLeft,
           Ogre::String fileCenter, Ogre::String fileRight,
           int x, int y, int w, int incMin)
{
   posX = x;
   posY = y;
   width = w;
   percentual = 0;
   
   imageLeft = new Image(ogreOverlay, fileLeft);
   imageCenter = new Image(ogreOverlay, fileCenter);
   imageRight = new Image(ogreOverlay, fileRight);
   
   minWidth = imageLeft->getWidth() + incMin;
   this->incMin = incMin;
}


/***********************************************************************
 *                            Destructor                               *
 ***********************************************************************/
Ibar::~Ibar()
{
   delete imageLeft;
   delete imageCenter;
   delete imageRight;
}

/***********************************************************************
 *                           setPercentual                             *
 ***********************************************************************/
void Ibar::setPercentual(float perc)
{
   /* Only need to change if different than actual */
   if(percentual != perc)
   {
      percentual = perc;
      int curWidth = Ogre::Math::Floor((perc)*width);
      
      if(perc == 0)
      {
         /* No images */
         hide();
         return;
      }
      
      if(curWidth <= imageLeft->getWidth()+imageRight->getWidth())
      {
         /* Just left and right images */
         if(curWidth < minWidth)
         {
            /* Lesser than min, must render as min. */
            curWidth = minWidth;
         }
         imageLeft->setPosition(posX, posY);
         imageLeft->show();
         imageCenter->hide();
         imageRight->setPosition(posX+(curWidth-imageRight->getWidth()),
                                 posY);
         imageRight->show();
         return;
      }
      else
      {
         imageLeft->setPosition(posX, posY);
         imageLeft->show();
         imageCenter->setPosition(posX+imageLeft->getWidth(), posY);
         int centerWidth = curWidth-minWidth-incMin;
         if(centerWidth < imageLeft->getWidth())
         {
            /* To avoid distortions, must not be lesser than
             * image width. */
            centerWidth = imageLeft->getWidth();
         }
         imageCenter->setDimensions(centerWidth,
                                    imageCenter->getHeight());
         imageCenter->show();
         imageRight->setPosition(posX+centerWidth+incMin,
                                 posY);
         imageRight->show();
         return;
      }
   }
}


/***********************************************************************
 *                                 hide                                *
 ***********************************************************************/
void Ibar::hide()
{
   imageCenter->hide();
   imageLeft->hide();
   imageRight->hide();
}

/***********************************************************************
 *                                 show                                *
 ***********************************************************************/
void Ibar::show()
{
   setPercentual(percentual);
}

/***********************************************************************
 *                              isVisible                              *
 ***********************************************************************/
bool Ibar::isVisible()
{
   return(imageLeft->isVisible() && imageRight->isVisible());
}
