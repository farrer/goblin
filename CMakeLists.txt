
cmake_minimum_required(VERSION 2.8)

# Define project and its current version
set(PACKAGE goblin)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)
if(${VERSION_PATCH})
   set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
else(${VERSION_PATCH})
   set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
endif(${VERSION_PATCH})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

# Define some options
option(GOBLIN_STATIC "Static build" FALSE)
option(GOBLIN_DEBUG "Enable debug symbols" FALSE)

# Some compiler options
if(UNIX)
   if(${GOBLIN_DEBUG})
      add_definitions(-Wall -g)
   else(${GOBLIN_DEBUG})
      add_definitions(-Wall)
   endif(${GOBLIN_DEBUG})
endif(UNIX)

if(APPLE)
   set(GOBLIN_STATIC True)
   set(OGRE_STATIC True)
endif(APPLE)

#Find Packages
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
   # For android, must build statically.
   set(GOBLIN_STATIC True)
   set(OGRE_STATIC True)
   # Use Tremor instead of Vorbis and Vorbis File
   FIND_PACKAGE(Tremor REQUIRED)
   include_directories(${VORBISFILE_INCLUDE_DIR})
else(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
   # SDL2 is only used on non android systems
   FIND_PACKAGE(SDL2 REQUIRED)
   include_directories(${SDL2_INCLUDE_DIR})
   # Vorbis and vorbis file 
   FIND_PACKAGE(Vorbis REQUIRED)
   include_directories(${VORBIS_INCLUDE_DIR})
   FIND_PACKAGE(Vorbisfile REQUIRED)
   include_directories(${VORBISFILE_INCLUDE_DIR})
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")

FIND_PACKAGE(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})
FIND_PACKAGE(Ogg REQUIRED)
include_directories(${OGG_INCLUDE_DIR})
FIND_PACKAGE(OGRE REQUIRED)
include_directories(${OGRE_INCLUDE_DIR})
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
   include_directories(${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/)
   include_directories(${OGRE_INCLUDE_DIR}/RenderSystems/GLES2/Android/)
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
FIND_PACKAGE(Kobold REQUIRED)
include_directories(${KOBOLD_INCLUDE_DIR})
FIND_PACKAGE(Kosound REQUIRED)
include_directories(${KOSOUND_INCLUDE_DIR})

#FIXME: .so is too unix like. fix for other systems.
# Define our render systems directories
if(${GOBLIN_DEBUG})
   string(REPLACE ".so" "" OGRE_GL_RENDER_SYSTEM_LIB 
          ${OGRE_RenderSystem_GL_LIBRARY_DBG})
   string(REPLACE ".so" "" OGRE_GL3PLUS_RENDER_SYSTEM_LIB 
          ${OGRE_RenderSystem_GL3Plus_LIBRARY_DBG})
else(${GOBLIN_DEBUG})
   string(REPLACE ".so" "" OGRE_GL_RENDER_SYSTEM_LIB 
          ${OGRE_RenderSystem_GL_LIBRARY_REL})
   string(REPLACE ".so" "" OGRE_GL3PLUS_RENDER_SYSTEM_LIB 
          ${OGRE_RenderSystem_GL3Plus_LIBRARY_REL})
endif(${GOBLIN_DEBUG})

# Include directories: src and the binary src (as generating config.h there)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

# Generate dynamic info
set(GOBLIN_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/goblinconfig.h)
configure_file("./src/goblinconfig.h.in" "./src/goblinconfig.h")

# Include headers and files
include(sources.cmake)
  

# Build the library with defined mode (static or shared)
if(${GOBLIN_STATIC})
   add_library(goblin ${GOBLIN_SOURCES} ${GOBLIN_HEADERS})
else(${GOBLIN_STATIC})
   add_library(goblin SHARED ${GOBLIN_SOURCES} ${GOBLIN_HEADERS} )
endif(${GOBLIN_STATIC})

set_target_properties(goblin PROPERTIES VERSION ${VERSION}
                             SOVERSION ${VERSION_MAJOR} )

# install the include files and created library.
install(FILES ${GOBLIN_CONFIG_FILE} DESTINATION include/goblin)
install(FILES ${GOBLIN_HEADERS} DESTINATION include/goblin)
install(TARGETS goblin DESTINATION lib)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
   set(JAVA_FILES src/jni/ActivityJNI.java src/jni/MainJNIActivity.java)
   install(FILES ${JAVA_FILES} DESTINATION share/goblin)
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")


