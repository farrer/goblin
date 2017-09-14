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

#ifndef _ibar_h
#define _ibar_h

#include <OGRE/OgrePrerequisites.h>
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

#include "guiobject.h"
#include "image.h"

namespace Goblin
{

/*! Class that implements a progress bar 
 * \note deprecated: Should use Farso instead.*/
class Ibar
{
   public:
      /*! Constructor 
       * \param fileLeft left image filename 
       * \param fileCenter center image filename
       * \param fileRight right image filename
       * \param x x coordinate of the bar
       * \param y y coordinate of the bar
       * \param w width of the bar 
       * \param value to  increment for imageLeft width for minimun
       *        valid Ibar width (usually, the minimun delta for images
       *        not overlap badly). */
      Ibar(Ogre::Overlay* ogreOverlay,
           const Ogre::String& fileLeft, const Ogre::String& fileCenter,
           const Ogre::String& fileRight, int x, int y, int w, int incMin,
           const Ogre::String& groupName);
      /*! Destructor */
      ~Ibar();
   
      /*! Set current displayed percentual.
       * \param perc current percentual [0..100] */
      void setPercentual(float perc);
   
      /*! Hide the bar */
      void hide();
   
      /*! Show the bar */
      void show();
   
      /*! Verify if the bar is visible or not */
      bool isVisible();
   
   
   protected:
   
      Image* imageLeft;    /**< Current left image */
      Image* imageCenter;  /**< Current center image */
      Image* imageRight;   /**< Current right image */
   
      int percentual; /**< Current displayed percentual */
      int posX; /**< current x position */
      int posY; /**< current z position */
      int width; /**< max width of bar */
      int minWidth; /**< max width of bar */
      int incMin; /**< incMin size defined (see constructor) */

};

}

#endif

#endif


