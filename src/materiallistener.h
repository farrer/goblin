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

#ifndef _goblin_material_listener_h
#define _goblin_material_listener_h

#include <OGRE/Prerequisites.h>

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)

#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreLogManager.h>

namespace Goblin
{

/*! Listener to force use of GLSL instead of fixed pipeline for materials */
class MaterialListener: public Ogre::MaterialManager::Listener
{
   public:
      /*! Constructor.
       * \param pShaderGenerator -> pointer to the shader generator used. */
      MaterialListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator);
      
      /*! Destructor. */
      virtual ~MaterialListener();

      /*! This is the hook point where shader based technique will be created.
       *  It will be called whenever the material manager won't find 
       * appropriate technique that satisfy the target scheme name. If the 
       * scheme name is out target RT Shader System scheme name we will try 
       * to create shader generated technique for it. */
      virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
            const Ogre::String& schemeName, Ogre::Material* originalMaterial, 
            unsigned short lodIndex, const Ogre::Renderable* rend);

      virtual bool afterIlluminationPassesCreated(Ogre::Technique* tech);

      virtual bool beforeIlluminationPassesCleared(Ogre::Technique* tech);

   private:
      /*! The shader generator used.*/
      Ogre::RTShader::ShaderGenerator* shaderGenerator;

};

}

#endif

#endif


