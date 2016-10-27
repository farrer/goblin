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

#if KOBOLD_PLATFORM != KOBOLD_PLATFORM_IOS &&\
    KOBOLD_PLATFORM != KOBOLD_PLATFORM_ANDROID


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
   Kobold::Mouse::update();

   /* Update mouse container rendererer */
   ogreCursorContainer->setPosition(getX() / windowX, getY() / windowY);
}

/*****************************************************************
 *                         updateByEvent                         *
 *****************************************************************/
void Cursor::updateByEvent(SDL_Event event)
{
   Kobold::Mouse::updateByEvent(event);
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getX()
{
   return Kobold::Mouse::getX();
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getY()
{
   return Kobold::Mouse::getY();
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getRelativeX()
{
   return Kobold::Mouse::getRelativeX();
}

/*****************************************************************
 *                             getX                              *
 *****************************************************************/
int Cursor::getRelativeY()
{
   return Kobold::Mouse::getRelativeY();
}

/*****************************************************************
 *                        getRelativeWheel                       *
 *****************************************************************/
int Cursor::getRelativeWheel()
{
   return Kobold::Mouse::getRelativeWheel();
}

/*****************************************************************
 *                       isLeftButtonPressed                     *
 *****************************************************************/
bool Cursor::isLeftButtonPressed()
{
   return Kobold::Mouse::isLeftButtonPressed();
}

/*****************************************************************
 *                      isMiddleButtonPressed                    *
 *****************************************************************/
bool Cursor::isMiddleButtonPressed()
{
   return Kobold::Mouse::isMiddleButtonPressed();
}

/*****************************************************************
 *                      isRightButtonPressed                     *
 *****************************************************************/
bool Cursor::isRightButtonPressed()
{
   return Kobold::Mouse::isRightButtonPressed();
}

/*****************************************************************
 *                       checkButtonRelease                      *
 *****************************************************************/
bool Cursor::checkButtonRelease(int buttonNumber)
{
   return Kobold::Mouse::checkButtonRelease(buttonNumber);
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

}


#endif

