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

#include "textbox.h"
#include "screeninfo.h"

namespace Goblin
{

/***********************************************************************
 *                              constructor                            *
 ***********************************************************************/
TextBox::TextBox(int x, int y, int w, int h, Ogre::String text, 
                 Ogre::String name, Ogre::Overlay* ogreOverlay,
                 Ogre::String fontName, Ogre::Real charHeight)
        :GuiObject(ogreOverlay, GuiObject::TYPE_TEXT_BOX, "TextArea", name)
{
   guiOverlay = ogreOverlay;

   dispText = (Ogre::TextAreaOverlayElement*) container; 
   dispText->setMetricsMode(Ogre::GMM_PIXELS);
   setPosition(x, y);
   setDimensions(w, h);
   dispText->setFontName(fontName);
   if(ScreenInfo::shouldUseDoubleSizedGui())
   {
      dispText->setCharHeight(charHeight * 2);
   }
   else
   {
      dispText->setCharHeight(charHeight);
   }

   dispText->setCaption(text);

   deltaAlpha = 0.0f;
   color.r = 0.0f;
   color.g = 0.0f;
   color.b = 0.0f;
   color.a = 1.0f;
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
TextBox::~TextBox()
{
}

/***********************************************************************
 *                                setText                              *
 ***********************************************************************/
void TextBox::setText(Ogre::String text)
{
   if(dispText->getCaption().compare(text) != 0)
   {
      dispText->setCaption(text);
   }
}

/***********************************************************************
 *                                getText                              *
 ***********************************************************************/
Ogre::String TextBox::getText()
{
   if(dispText != NULL)
   {
      return dispText->getCaption();
   }

   return "";
}

/***********************************************************************
 *                                setFont                              *
 ***********************************************************************/
void TextBox::setFont(Ogre::String fontName, Ogre::Real size)
{
   dispText->setFontName(fontName);
   if(ScreenInfo::shouldUseDoubleSizedGui())
   {
      dispText->setCharHeight(size * 2);
   }
   else
   {
      dispText->setCharHeight(size);
   }
}

/***********************************************************************
 *                               setColor                              *
 ***********************************************************************/
void TextBox::setColor(Ogre::Real r, Ogre::Real g, Ogre::Real b, Ogre::Real a)
{
   color.r = r;
   color.g = g;
   color.b = b;
   color.a = a;
   dispText->setColour(color);
}

/***********************************************************************
 *                            setAlignment                             *
 ***********************************************************************/
void TextBox::setAlignment(Ogre::TextAreaOverlayElement::Alignment a)
{
   dispText->setAlignment(a);
}

/***********************************************************************
 *                                fadeIn                               *
 ***********************************************************************/
void TextBox::fadeIn()
{
   if(!isVisible())
   {
      /* make the text visible */
      show();
   }

   /* Set alpha and delta to start effect */
   color.a = 0.0f;
   dispText->setColour(color);
   deltaAlpha = 0.05f;
   needUpdate = true;
}

/***********************************************************************
 *                                fadeOut                              *
 ***********************************************************************/
void TextBox::fadeOut()
{
   /* no fade-out on invisible text */
   if(isVisible())
   {
      /* Set alpha and delta to start effect */
      color.a = 1.0f;
      dispText->setColour(color);
      deltaAlpha = -0.05f;
      needUpdate = true;
   }
}


/***********************************************************************
 *                               _update                               *
 ***********************************************************************/
void TextBox::_update()
{
   if(deltaAlpha != 0.0f)
   {
      if(deltaAlpha < 0)
      {
         color.a += deltaAlpha;
         if(color.a > 0)
         {
            dispText->setColour(color);
         }
         else
         {
            /* fade out finished! */
            hide();
            deltaAlpha = 0.0f;
            /* Reset alpha to avoid show with full blend */
            color.a = 1.0f; 
            dispText->setColour(color);
         }
      }
      else /* deltaAlpha > 0 */
      {
         color.a += deltaAlpha;
         if(color.a > 1.0f)
         {
            color.a = 1.0f;
            deltaAlpha = 0.0f;
         }
         dispText->setColour(color);
      }

      /* Update the need to update (metalinguistic it, uhm?) */
      needUpdate |= (deltaAlpha != 0.0f);
   }
}

}

