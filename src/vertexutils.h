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

#ifndef _goblin_vertex_utils_h
#define _goblin_vertex_utils_h

#include <OGRE/OgrePrerequisites.h>

#if OGRE_VERSION_MAJOR >= 2

#include <OGRE/Hash/MurmurHash3.h>
#include <OGRE/OgreVector2.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/Vao/OgreVertexBufferPacked.h>

namespace Goblin
{
   /*! A vertex utils class, with some functions to work on vertices.
    * \note all of them are from Dergo server project, which is MIT licensed,
    *       Copyright (c) 2015 Matias N. Goldberg.
    *       See: https://bitbucket.org/dark_sylinc/dergo-blender/src/ */
   class VertexUtils
   {
      /** Generates tangents for normal mapping for indexed lists version. 
       * Step 01. The data will be stored into outData.
       * @remarks Tangent generation for indexed lists is split in two 
       * steps to allow some degree of threading, since step 01 can be 
       * done concurrently, while step 02 cannot.
       * To generate tangents on a single threading app, do the following:
       * Ogre::Vector3 *tuvData = new Ogre::Vector3[numVertices * 2u];
       * generateTanUV( vertexData, indexData, bytesPerVertex,
       *                numVertices, numIndices, posStride,
       *                normalStride, tangentStride, uvStride,
       *                outData );
       * generateTangentsMergeTUV( vertexData, bytesPerVertex, numVertices,
       *                          normalStride, tangentStride, outData, 1 );
       * delete [] outData;
       * @param outData
       * Buffer to store the result of this step. It generates vectors 'tsU' 
       * and 'tsV', one pair for each vertex. outData must be at least 
       * outData = new Vector3[numVertices*2u]  */
      static void generateTanUV( const uint8_t* vertexData, 
            const uint32_t* indexData, uint32_t bytesPerVertex,
            uint32_t numVertices, uint32_t numIndices, uint32_t posStride,
            uint32_t normalStride, uint32_t tangentStride, uint32_t uvStride,
            Ogre::Vector3* RESTRICT_ALIAS outData );

      /** Generates tangents for normal mapping for indexed lists version. 
       * Step 02 and final step.
       * Takes the data in inOutUvBuffer (which was the output of 
       * generateTanTUV) and fills the vertexData.
       * @param inOutUvBuffer
       *    The buffer filled with tsU & tsV by generateTanUV. Assumes all 
       *    threads are consecutive. This means the buffer must be at least 
       *    new Vector3[numVertices*2u*numThreads]
       * @param numThreads
       *    The number of threads that processed generateTanUV. */
   static void generateTangentsMergeTUV(uint8_t* vertexData, 
         uint32_t bytesPerVertex, uint32_t numVertices, uint32_t normalStride,
         uint32_t tangentStride, Ogre::Vector3* RESTRICT_ALIAS inOutUvBuffer,
         size_t numThreads );
};

}

#endif

#endif

