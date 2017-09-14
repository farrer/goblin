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

#include "image.h"
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)


#include "screeninfo.h"

#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/OgreLog.h>
#include <OGRE/OgreLogManager.h>

namespace Goblin
{

/***********************************************************************
 *                                Constructor                          *
 ***********************************************************************/
Image::Image(Ogre::Overlay* ogreOverlay, const Ogre::String& imageFileName,
      const Ogre::String& groupName)
   :GuiObject(ogreOverlay, GuiObject::TYPE_IMAGE, "Panel", imageFileName)
{
   init(imageFileName, groupName);
}
   
/***********************************************************************
 *                                Constructor                          *
 ***********************************************************************/
Image::Image(int guiObjectType, Ogre::Overlay* ogreOverlay,
             const Ogre::String& imageFileName, const Ogre::String& groupName)
   :GuiObject(ogreOverlay, guiObjectType, "Panel", imageFileName)
{
   init(imageFileName, groupName);
}
   
/***********************************************************************
 *                                   init                              *
 ***********************************************************************/
void Image::init(const Ogre::String& imageFileName, 
      const Ogre::String& groupName)
{
   Ogre::ResourceManager::ResourceCreateOrRetrieveResult matRes;
   imageFile = imageFileName;
   alpha = 1.0f;
   deltaAlpha = 0.0f;
   
   /* Create material */
   matRes = Ogre::MaterialManager::getSingleton().createOrRetrieve(
         imageFileName, groupName);
   material = Ogre::static_pointer_cast<Ogre::Material>(matRes.first);
   
   /* Load the image, if the material was created. */
   if(matRes.second)
   {
      load(groupName);
   }
   else
   {
      /* Texture already loaded, let's just retrieve it. */
      texture = Ogre::TextureManager::getSingleton().getByName(imageFile, 
            groupName);
   }
   container->setMaterialName(material->getName());


   /* Set container size, based on image size */
   container->setWidth(texture->getWidth() / ScreenInfo::getWindowWidth());
   container->setHeight(texture->getHeight() / ScreenInfo::getWindowHeight());
   setDimensions(texture->getWidth(), texture->getHeight());

}

/***********************************************************************
 *                                 Destructor                          *
 ***********************************************************************/
Image::~Image()
{
   /* Remove the material */
   //Ogre::MaterialManager::getSingleton().remove(material->getName());
}

/***********************************************************************
 *                               loadImage                             *
 ***********************************************************************/
void Image::load(const Ogre::String& groupName)
{
   texture = Ogre::TextureManager::getSingleton().load(imageFile, groupName,
      Ogre::TEX_TYPE_2D);
   
   if(!texture)
   {
      Ogre::LogManager::getSingleton().getDefaultLog()->stream(
            Ogre::LML_CRITICAL)
         << "Couldn't load texture: '"
         << imageFile << "'";
      return;
   }
   
   /* Get the technique to define */
   Ogre::Technique* tech = material->getTechnique(0);

   /* Verify if need to create a pass */
   int numPasses = tech->getNumPasses();
   if(numPasses == 0) 
   {
      tech->createPass();
   }

   
   /* Get or create new texture */
   int numTextures = tech->getPass(0)->getNumTextureUnitStates();
   if(numTextures != 0)
   {
      texState = tech->getPass(0)->getTextureUnitState(0);
   }
   else
   {
      texState = tech->getPass(0)->createTextureUnitState(texture->getName());
   }
   texState->setTexture(texture);

   /* Set Material things */
   texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
   if(texture->hasAlpha())
   {
      tech->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
      tech->getPass(0)->setDepthWriteEnabled(false);
      tech->getPass(0)->setDepthCheckEnabled(false);
   }
   else
   {
      tech->getPass(0)->setSceneBlending(Ogre::SBT_REPLACE);
   }
   tech->getPass(0)->setLightingEnabled(false);
   tech->getPass(0)->setCullingMode(Ogre::CULL_NONE);
   tech->getPass(0)->setColourWriteEnabled(true);
}

/***********************************************************************
 *                                fadeIn                               *
 ***********************************************************************/
void Image::fadeIn()
{
   if(!isVisible())
   {
      /* make the image visible */
      show();
   }

   /* Set alpha and delta to start effect */
   alpha = 0.0f;
   texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, 
         Ogre::LBS_TEXTURE, alpha);
   deltaAlpha = 0.05f;
   needUpdate = true;
}

/***********************************************************************
 *                                fadeOut                              *
 ***********************************************************************/
void Image::fadeOut()
{
   /* no fade-out on invisible images */
   if(isVisible())
   {
      /* Set alpha and delta to start effect */
      alpha = 1.0f;
      texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, 
            Ogre::LBS_TEXTURE, alpha);
      deltaAlpha = -0.05f;
      needUpdate = true;
   }
}

/***********************************************************************
 *                                 update                              *
 ***********************************************************************/
void Image::_update()
{
   if(deltaAlpha != 0.0f)
   {
      if(deltaAlpha < 0)
      {
         alpha += deltaAlpha;
         if(alpha > 0)
         {
            texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, 
                  Ogre::LBS_TEXTURE, alpha);
         }
         else
         {
            /* fade out finished! */
            hide();
            deltaAlpha = 0.0f;
            /* Reset alpha to avoid show with full blend */
            texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, 
                  Ogre::LBS_TEXTURE, 1.0f);
         }
      }
      else /* deltaAlpha > 0 */
      {
         alpha += deltaAlpha;
         if(alpha > 1.0f)
         {
            alpha = 1.0f;
            deltaAlpha = 0.0f;
         }
         texState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, 
               Ogre::LBS_TEXTURE, alpha);
      }

      /* Update the need to update (metalinguistic it, uhm?) */
      needUpdate |= (deltaAlpha != 0.0f);
   }
}

/***********************************************************************
 *                                getFileName                          *
 ***********************************************************************/
Ogre::String Image::getFileName()
{
   return imageFile;
}

}

#endif

