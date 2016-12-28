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


#include "vertexutils.h"

#if OGRE_VERSION_MAJOR >= 2

using namespace Goblin;

/***********************************************************************
 *                            generateTanUV                            *
 ***********************************************************************/
void VertexUtils::generateTanUV(const uint8_t* vertexData, 
      const uint32_t *indexData, uint32_t bytesPerVertex, uint32_t numVertices,
      uint32_t numIndices, uint32_t posStride, uint32_t normalStride, 
      uint32_t tangentStride, uint32_t uvStride, 
      Ogre::Vector3* RESTRICT_ALIAS outData)
{
   using namespace Ogre;
   Vector3* RESTRICT_ALIAS tsUs = outData;
   Vector3* RESTRICT_ALIAS tsVs = outData + numVertices;

   for( ::uint32_t i=0; i<numIndices; i += 3 )
   {
      /* Lengyel's Method, it is fast and simple. Also we don't need to 
       * care about duplicating vertices due to parity discontinuities or 
       * averaging tangents that are too different since the input assumes 3 
       * vertices per triangle.*/
      Vector3 const* RESTRICT_ALIAS vPos[3];
      Vector2 const* RESTRICT_ALIAS uv[3];

      for( int j=0; j<3; ++j )
      {
         vPos[j] = reinterpret_cast<Vector3 const* RESTRICT_ALIAS>(
               vertexData + posStride + indexData[i+j] * bytesPerVertex );
         uv[j] = reinterpret_cast<Vector2 const* RESTRICT_ALIAS>(
               vertexData + uvStride + indexData[i+j] * bytesPerVertex );
      }

      const Vector2 deltaUV1 = *uv[1] - *uv[0];
      const Vector2 deltaUV2 = *uv[2] - *uv[0];
      const Vector3 deltaPos1 = *vPos[1] - *vPos[0];
      const Vector3 deltaPos2 = *vPos[2] - *vPos[0];

      /* face normal */
      Vector3 tsN = deltaPos1.crossProduct(deltaPos2);
      tsN.normalise();

      const Real uvarea = deltaUV1.crossProduct(deltaUV2);
      if( !Math::RealEqual(uvarea, 0.0f) )
      {
         /* Normalise by uvarea */
         const Real a =  deltaUV2.y / uvarea;
         const Real b = -deltaUV1.y / uvarea;
         const Real c = -deltaUV2.x / uvarea;
         const Real d =  deltaUV1.x / uvarea;

         const Vector3 tsU = (deltaPos1 * a) + (deltaPos2 * b);
         const Vector3 tsV = (deltaPos1 * c) + (deltaPos2 * d);

         uint32_t idx = indexData[i+0];
         tsUs[idx] += tsU;
         tsVs[idx] += tsV;

         idx = indexData[i+1];
         tsUs[idx] += tsU;
         tsVs[idx] += tsV;

         idx = indexData[i+2];
         tsUs[idx] += tsU;
         tsVs[idx] += tsV;
      }
   }
}

/***********************************************************************
 *                       generateTangentsMergeUV                       *
 ***********************************************************************/
void VertexUtils::generateTangentsMergeTUV(uint8_t *vertexData, 
      uint32_t bytesPerVertex, uint32_t numVertices, uint32_t normalStride,
      uint32_t tangentStride, Ogre::Vector3* RESTRICT_ALIAS inOutUvBuffer, 
      size_t numThreads)
{
   using namespace Ogre;

   Vector3* RESTRICT_ALIAS tsUs = inOutUvBuffer;
   Vector3* RESTRICT_ALIAS tsVs = inOutUvBuffer + numVertices;

   for( ::uint32_t i=0; i<numVertices; ++i )
   {
      for( size_t j=1; j<numThreads; ++j )
      {
         /* Merge the tsU & tsV vectors calculated by the other threads 
          * for these same vertices */
         tsUs[i] += inOutUvBuffer[i + numVertices * 2u * j];
         tsVs[i] += inOutUvBuffer[i + numVertices * 2u * j + numVertices];
      }

      Vector3 const* RESTRICT_ALIAS vNormal;
      Vector3* RESTRICT_ALIAS vTangent;
      float* RESTRICT_ALIAS fParity;

      vNormal = reinterpret_cast<Vector3 const* RESTRICT_ALIAS>(
            vertexData + normalStride);
      vTangent	= reinterpret_cast<Vector3* RESTRICT_ALIAS>(
            vertexData + tangentStride);
      fParity = reinterpret_cast<float* RESTRICT_ALIAS>(
            vertexData + tangentStride + sizeof(Vector3));

      /* Gram-Schmidt orthogonalize */
      *vTangent = (tsUs[i] - (*vNormal) * vNormal->dotProduct( 
               tsUs[i])).normalisedCopy();

      /* Calculate handedness */
      *fParity = vNormal->crossProduct(tsUs[i]).dotProduct(tsVs[i]) < 0.0f ? 
         -1.0f : 1.0f;

      vertexData += bytesPerVertex;
   }
}

#endif

