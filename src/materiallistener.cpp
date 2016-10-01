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

#include "materiallistener.h"

using namespace Goblin;

/***********************************************************************
 *                             Constructor                             *
 ***********************************************************************/
MaterialListener::MaterialListener(Ogre::RTShader::ShaderGenerator* 
      pShaderGenerator)
{
   shaderGenerator = pShaderGenerator;
}

/***********************************************************************
 *                              Destructor                             *
 ***********************************************************************/
MaterialListener::~MaterialListener()
{
}

/***********************************************************************
 *                       handleSchemeNotFound                          *
 ***********************************************************************/
Ogre::Technique* MaterialListener::handleSchemeNotFound(
      unsigned short schemeIndex, const Ogre::String& schemeName,
      Ogre::Material* originalMaterial, unsigned short lodIndex, 
      const Ogre::Renderable* rend)
{
   Ogre::LogManager::getSingleton().getDefaultLog()->stream(
         Ogre::LML_NORMAL)
      << "Will create shader technique for scheme '"
      << schemeName << "' material '" << originalMaterial->getName()
      << "'";

   /* Case this is the default shader generator scheme. */
   if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
   {
      bool techniqueCreated;

      /* Create shader generated technique for this material. */
      techniqueCreated = shaderGenerator->createShaderBasedTechnique(
            originalMaterial->getName(), 
            Ogre::MaterialManager::DEFAULT_SCHEME_NAME, schemeName);   

      /* Case technique registration succeeded. */
      if(techniqueCreated)
      {
         /* Force creating the shaders for the generated technique. */
         shaderGenerator->validateMaterial(schemeName, 
               originalMaterial->getName());

         /* Grab the generated technique.*/
         Ogre::Material::TechniqueIterator itTech = 
            originalMaterial->getTechniqueIterator();

         while(itTech.hasMoreElements())
         {
            Ogre::Technique* curTech = itTech.getNext();

            if (curTech->getSchemeName() == schemeName)
            {
               return curTech;
            }
         }            
      }
   }
   Ogre::LogManager::getSingleton().getDefaultLog()->stream(
         Ogre::LML_CRITICAL)
      << "Couldn't : create shader technique for scheme '"
      << schemeName << "' material '" << originalMaterial->getName()
      << "'";

   return NULL;
}

/***********************************************************************
 *                    afterIlluminationPassesCreated                   *
 ***********************************************************************/
bool MaterialListener::afterIlluminationPassesCreated(Ogre::Technique* tech)
{
   if(tech->getSchemeName() == 
         Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
   {
      Ogre::Material* mat = tech->getParent();
      shaderGenerator->validateMaterialIlluminationPasses(
            tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
   }
   return false;
}

/***********************************************************************
 *                    afterIlluminationPassesCleared                   *
 ***********************************************************************/
bool MaterialListener::beforeIlluminationPassesCleared( Ogre::Technique* tech)
{
   if(tech->getSchemeName() == 
         Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
   {
      Ogre::Material* mat = tech->getParent();
      shaderGenerator->invalidateMaterialIlluminationPasses(
            tech->getSchemeName(), mat->getName(), mat->getGroup());
      return true;
   }
   return false;
}




