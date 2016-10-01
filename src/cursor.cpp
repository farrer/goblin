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

#include "cursor.h"
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
namespace Goblin
{

/*****************************************************************
 *                         Constructor                           *
 *****************************************************************/
void Cursor::init(Ogre::RenderWindow* ogreWindow)
{
   /* Disable HW Mouse cursos */
   SDL_ShowCursor(SDL_DISABLE);

   /* Set window dimensions */
   windowX = ogreWindow->getWidth();
   windowY = ogreWindow->getHeight();

   /* Create Material */
   cursorMaterial = Ogre::MaterialManager::getSingleton().create(
         "mouseCursor", "gui");

   /* Create the container to mouse */
   ogreCursorContainer = (Ogre::OverlayContainer*)
      Ogre::OverlayManager::getSingletonPtr()->createOverlayElement("Panel",
            "nouseCursor");
   ogreCursorContainer->setMaterialName(cursorMaterial->getName());
   ogreCursorContainer->setPosition(0, 0);

   /* FIXME: overlay must be generic for all gui, not inner here. */
   ogreGuiOverlay = Ogre::OverlayManager::getSingletonPtr()->create("mouse");
   ogreGuiOverlay->setZOrder(649);
   ogreGuiOverlay->add2D(ogreCursorContainer);
   ogreGuiOverlay->show();

   /* Load Cursors */
   set("cursor/sel.png");
}

/*****************************************************************
 *                          Destructor                           *
 *****************************************************************/
void Cursor::finish()
{
   /* Renable HW Mouse cursos */
   SDL_ShowCursor(SDL_ENABLE);

   /* Remove from the overlay */
   ogreGuiOverlay->remove2D(ogreCursorContainer);
   Ogre::OverlayManager::getSingletonPtr()->destroyOverlayElement(
         ogreCursorContainer);
   /* Destroy the gui overlay */
   Ogre::OverlayManager::getSingletonPtr()->destroy(ogreGuiOverlay);

   /* Remove Shaders from material */
   Ogre::RTShader::ShaderGenerator::getSingleton().
            removeAllShaderBasedTechniques(cursorMaterial->getName());

   /* Destroy the material and current texture (also, undefining the 
    * static pointers, to avoid errors on OgreRoot destroy (of persistent 
    * references still active). */
   Ogre::MaterialManager::getSingleton().remove(cursorMaterial->getName());
   cursorMaterial.setNull();
   Ogre::TextureManager::getSingleton().remove(cursorTexture->getName());
   cursorTexture.setNull();
}

/*****************************************************************
 *                         Load Cursor                           *
 *****************************************************************/
void Cursor::loadCursor(Ogre::String fileName)
{
   cursorTexture = Ogre::TextureManager::getSingleton().load(fileName, "gui");

   /* Get or create new texture */
   Ogre::TextureUnitState *pTexState;
   if(cursorMaterial->getTechnique(0)->getPass(0)->getNumTextureUnitStates())
   {
      pTexState = cursorMaterial->getTechnique(0)->getPass(0)->
         getTextureUnitState(0);
   }
   else
   {
      pTexState = cursorMaterial->getTechnique(0)->getPass(0)->
         createTextureUnitState(cursorTexture->getName());
   }

   /* Set Material things */
   pTexState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
   cursorMaterial->getTechnique(0)->getPass(0)->
      setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);

   /* Set Cursor size */
   ogreCursorContainer->setWidth(cursorTexture->getWidth() / windowX);
   ogreCursorContainer->setHeight(cursorTexture->getHeight() / windowY);
}

/*****************************************************************
 *                           update                              *
 *****************************************************************/
void Cursor::update()
{
   /* Get mouse state */
   relMouseX = mouseX;
   relMouseY = mouseY;
   mButton = SDL_GetMouseState(&mouseX, &mouseY);
   relMouseX -= mouseX;
   relMouseY -= mouseY;

   /* Clear event related states */
   mouseWheel = 0;
   for(int i = 0; i < MAX_MOUSE_BUTTONS; i++)
   {
      mouseButtonReleased[i] = false;
   }

   /* Update mouse container rendererer */
   ogreCursorContainer->setPosition(mouseX / windowX, mouseY / windowY);
}

/*****************************************************************
 *                         updateByEvent                         *
 *****************************************************************/
void Cursor::updateByEvent(SDL_Event event)
{
   if(event.type == SDL_MOUSEWHEEL)
   {
      /* Add or dec the amount moved */
      mouseWheel += event.wheel.y;
   }
   else if(event.type == SDL_MOUSEBUTTONUP)
   {
      if(event.button.button <= MAX_MOUSE_BUTTONS)
      {
         mouseButtonReleased[event.button.button - 1] = true;
      }
   }
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getX()
{
   return mouseX;
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getY()
{
   return mouseY;
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getRelativeX()
{
   return relMouseX;
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getRelativeY()
{
   return relMouseY;
}

/*****************************************************************
 *                        getRelativeWheel                       *
 *****************************************************************/
int Cursor::getRelativeWheel()
{
   return mouseWheel;
}

/*****************************************************************
 *                       isLeftButtonPressed                     *
 *****************************************************************/
bool Cursor::isLeftButtonPressed()
{
   return mButton & SDL_BUTTON(1);
}

/*****************************************************************
 *                      isMiddleButtonPressed                    *
 *****************************************************************/
bool Cursor::isMiddleButtonPressed()
{
   return mButton & SDL_BUTTON(2);
}

/*****************************************************************
 *                      isRightButtonPressed                     *
 *****************************************************************/
bool Cursor::isRightButtonPressed()
{
   return mButton & SDL_BUTTON(3);
}

/*****************************************************************
 *                       checkButtonRelease                      *
 *****************************************************************/
bool Cursor::checkButtonRelease(int buttonNumber)
{
   if(buttonNumber < MAX_MOUSE_BUTTONS)
   {
      return mouseButtonReleased[buttonNumber];
   }

   return false;
}

/*****************************************************************
 *                              set                              *
 *****************************************************************/
void Cursor::set(Ogre::String fileName)
{
   loadCursor(fileName);
}

/*****************************************************************
 *                             Members                           *
 *****************************************************************/
Ogre::Overlay* Cursor::ogreGuiOverlay;
Ogre::OverlayContainer* Cursor::ogreCursorContainer;
Ogre::TexturePtr Cursor::cursorTexture;
Ogre::MaterialPtr Cursor::cursorMaterial;
Ogre::Real Cursor::windowX;
Ogre::Real Cursor::windowY;
Uint8 Cursor::mButton;
int Cursor::mouseX = -1;
int Cursor::mouseY = -1;
int Cursor::relMouseX = 0;
int Cursor::relMouseY = 0;
int Cursor::mouseWheel = 0;
bool Cursor::mouseButtonReleased[MAX_MOUSE_BUTTONS];

}
