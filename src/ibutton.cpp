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

#include "ibutton.h"
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

#include <kosound/sound.h>

using namespace Goblin;

#define DEFAULT_SIZE 32
#define DEFAULT_TEXT_DELTA -4
#define DEFAULT_OVER_DELTA 1

/***********************************************************************
 *                           Constructor                               *
 ***********************************************************************/
Ibutton::Ibutton(Ogre::Overlay* ogreOverlay, Ogre::String imageFileName)
        :Image(GuiObject::TYPE_IBUTTON, ogreOverlay, imageFileName)
{
   setDefaults();
   text = NULL;
}

/***********************************************************************
 *                           Constructor                               *
 ***********************************************************************/
Ibutton::Ibutton(Ogre::Overlay* ogreOverlay, Ogre::String imageFileName,
                 Ogre::String text, Ogre::String fontName,
                 Ogre::Real charHeight)
:Image(GuiObject::TYPE_IBUTTON, ogreOverlay, imageFileName)
{
   setDefaults();
   this->text = new TextBox(getWidth() / 2.0f, 0, getWidth(), 16, text,
                            "text"+imageFileName, ogreOverlay,
                            fontName, charHeight);
   this->text->setAlignment(Ogre::TextAreaOverlayElement::Center);
   this->text->setColor(1.0f, 1.0f, 1.0f, 1.0f);
   addChild(this->text, getWidth() / 2.0f, defaultHeight + DEFAULT_TEXT_DELTA);
}

/***********************************************************************
 *                           setDefaults                               *
 ***********************************************************************/
void Ibutton::setDefaults()
{
   defaultWidth = getWidth();
   defaultHeight = getHeight();
   overHeight = defaultHeight + DEFAULT_OVER_DELTA;
   overWidth = defaultWidth + DEFAULT_OVER_DELTA;
   wasPressed = false;
   havePressedSound = false;
   pressedSound = "";
}

/***********************************************************************
 *                            Destructor                               *
 ***********************************************************************/
Ibutton::~Ibutton()
{
   if(text != NULL)
   {
      delete text;
   }
}

/***********************************************************************
 *                       setMouseOverDimensions                        *
 ***********************************************************************/
void Ibutton::setMouseOverDimensions(Ogre::Real w, Ogre::Real h)
{
   overWidth = w;
   overHeight = h;
}

/***********************************************************************
 *                          setDimensions                              *
 ***********************************************************************/
void Ibutton::setDimensions(Ogre::Real w, Ogre::Real h)
{
   /* Set defaults */
   defaultWidth = w;
   defaultHeight = h;
   
   if(text != NULL)
   {
      GuiObjectChild* textChild = getChild(this->text);
      if(textChild != NULL)
      {
         textChild->offset = Ogre::Vector2(w / 2.0f, h + DEFAULT_TEXT_DELTA);
         textChild->resetPosition(getPosX(), getPosY());
      }
   }

   /* Set the mouse over dimensions */
   setMouseOverDimensions(w + (w / DEFAULT_SIZE) * 2,
                          h + (h / DEFAULT_SIZE) * 2);

   /* And set as image dimensions */
   Image::setDimensions(w, h);
}

/***********************************************************************
 *                             setText                                 *
 ***********************************************************************/
void Ibutton::setText(Ogre::String str)
{
   if(text != NULL)
   {
      text->setText(str);
   }
}

/***********************************************************************
 *                             getText                                 *
 ***********************************************************************/
Ogre::String Ibutton::getText()
{
   if(text != NULL)
   {
      return text->getText();
   }
   return "";
}

/***********************************************************************
 *                          setPressedSound                            *
 ***********************************************************************/
void Ibutton::setPressedSound(Ogre::String soundFile)
{
   pressedSound = soundFile;
   havePressedSound = (pressedSound != "");
}

/***********************************************************************
 *                          verifyEvents                               *
 ***********************************************************************/
int Ibutton::verifyEvents(int mouseX, int mouseY, bool leftButtonPressed)
{
   int event = IBUTTON_EVENT_NONE;

   if(container->isVisible())
   {
      /* Verify if mouse is inner the image or not */
      if(isPointIn(mouseX, mouseY))
      {
         if(leftButtonPressed)
         {
            /* Inner and pressing */
            event = IBUTTON_EVENT_ON_PRESS;
            wasPressed = true;
         }
         else if(wasPressed)
         {
            /* Pressed and released inner... pressed event! */
            wasPressed = false;
            event = IBUTTON_EVENT_PRESSED;
            if(havePressedSound)
            {
               Kosound::Sound::addSoundEffect(SOUND_NO_LOOP, pressedSound);
            }
         }
         else
         {
            /* Just inner */
            event = IBUTTON_EVENT_MOUSE_OVER;
         }
         /* Verify if need to grow-up */
         if(getWidth() == defaultWidth)
         {
            /* Must grow/shrink to the over dimensions */
            setTargetDimensions(overWidth, overHeight, 4);
         }
      }
      else
      {
         wasPressed = false;
         if((getWidth() != defaultWidth) && (!isUpdating()))
         {
            /* Must shrink/grow back to default dimensions */
            setTargetDimensions(defaultWidth, defaultHeight);
         }
      }
   }

   /* Update the image */
   update();

   return(event);
}

#endif

