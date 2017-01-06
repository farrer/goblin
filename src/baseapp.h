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

#ifndef _goblin_base_app_h
#define _goblin_base_app_h

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreTimer.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreRenderWindow.h>
#include <OGRE/Overlay/OgreOverlay.h>
#include <OGRE/Overlay/OgreOverlaySystem.h>
#include <OGRE/Overlay/OgreOverlayManager.h>

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   #include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
   #include "materiallistener.h"
#else
   #include <OGRE/Hlms/Unlit/OgreHlmsUnlit.h>
   #include <OGRE/Hlms/Pbs/OgreHlmsPbs.h>
   #include <OGRE/OgreHlmsManager.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || \
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   #include <kobold/multitouchcontroller.h>
#else
   #include <SDL2/SDL.h>
   #include <kobold/mouse.h>
   #include <kobold/keyboard.h>
#endif

#include "goblinconfig.h"
#include "fpsdisplay.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   #include <jni.h>
   #include <EGL/egl.h>
   #include <android/api-level.h>
   #include <android/native_window_jni.h>
   #include <android/asset_manager.h>
   #include <android/asset_manager_jni.h>
   #include <OGRE/RenderSystems/GLES2/OgreGLRenderSystemCommon.h>
   #include <OGRE/Android/OgreAPKFileSystemArchive.h>
   #include <OGRE/Android/OgreAPKZipArchive.h>
   #include <OGRE/OgreArchiveManager.h>
#endif

#if OGRE_VERSION_MAJOR != 1
   #include <OGRE/Compositor/OgreCompositorWorkspace.h>
#endif

namespace Goblin
{

/*! The basic class for all applications. This class implements - and 
 * abstract - the initialization and runing for all supported targets. */
class BaseApp
{
   public:
      /*! Constructor */
      BaseApp();
      /*! Destructor */
      virtual ~BaseApp();

      /*! Create root. Usually only publically called on Android platforms.
      * On all others, this function is called internally at #create. */
      bool createRoot();

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      /*! Init the APK asset manager for Android. 
       * \param env the used JNI enviroment 
       * \param assetManager used android asset manager. 
       * \param userHome location of directory to save user files.
       * \param cacheDir locaion of directory to save temporary files. */
      void initAssetManager(JNIEnv* env, jobject assetManager,
                            jstring userHome, jstring cacheDir);

      /*! Init the GLES2 window from a created andoid window surface. 
       * \param env the used JNI enviroment 
       * \param surface pointer to created surface window on android */
      void initWindowForAndroid(JNIEnv* env, jobject surface);

      /*! Terminate the Android Window used. */
      void termWindowForAndroid();

      /*! Do the android render frame. 
       * \param env the used JNI enviroment
       * \param vm pointer to used Java Virtual Machine */
      void renderOneFrameForAndroid(JNIEnv* env, JavaVM* vm);
#endif

      /*! Create and setup the system to use
       * \param userHome path to the know user home. "" to check for it.
       * \return true on success */
      bool create(Ogre::String userHome="", Ogre::uint32 wX=320,
                  Ogre::uint32 wZ=480, Ogre::Real wScale=1.0f);
 
      /*! Run the game.
       * \note -> for android and iOS it's just a single step, 
       * for other systems, it's the main loop. */
      void run();
   
      /*! Clean memory when receive low memory warning from system. */
      void lowMemoryClean();
  
      /*! Tell the application that it is running in the background */
      void sendToBackground();

      /*! Tell the application that it now is running in the foreground */
      void sendToForeground();

      /*! Retrieve current ogre window used. */
      Ogre::RenderWindow* getWindow();

      /*! Get the scene manager */
      Ogre::SceneManager* getSceneManager();

   protected:

      /*! \return application Unix name (all lower case, without spaces). */
      virtual const Ogre::String getApplicationUnixName() const = 0;

      /*! \return base data directory name (usually "data"). */
      virtual const Ogre::String getBaseDataDir() const = 0;

      /*! Do specific initialization for the implementation.
       * \return true on success, false to abort the initialization. */
      virtual bool doInit()=0;

      /*! Define all directories to use as Ogre resources, with
       * its unique group names. */
      virtual void getDataDirectories(Ogre::String** dataDirectories, 
            Ogre::String** dataGroups, int& total)=0;

      /*! \return default width to create window (ignored on embeded systems) */
      virtual const int getDefaultWindowWidth() const = 0;
      /*! \return default height to create window 
       * (ignored on embeded systems) */
      virtual const int getDefaultWindowHeight() const = 0;

      /*! Clean all possible not used memory. This function is called 
       * when the embeded system is with low available memory, and should
       * do it the best as it can. */
      virtual void doLowMemoryClean()=0;

      /*! Save current state, do some pauses: the application will be sent
       * to backgroun when this function is called. */
      virtual void doSendToBackground()=0;
      /*! Restore some state, resume the application: it will be sent to 
       * foreground when this function is called. */
      virtual void doSendToForeground()=0;

      /*! \return if should use Kobold::I18n for internationalization. */
      virtual const bool shouldUseKoboldI18n() const { return true; };


#if OGRE_VERSION_MAJOR == 1
      /*! \return Shadow Technique to use.
       * \note defaults to Ogre::SHADOWTYPE_NONE. */ 
      virtual Ogre::ShadowTechnique getShadowTechnique();
#else
      /*! \return if should create and define a basic ogre workspace.
       * \note if false, the implementation is responsible to define its own.*/
      virtual const bool shouldCreateBasicWorkspace() const { return true; };
#endif

      enum Orientation
      {
         LANDSCAPE,
         PORTRAIT
      };
      /*! \return default orientation the application will use. All goblin
       * applications are defined as single oriented, ie, the orientation 
       * is hardly defined at the init and screen rotation is disabled. */
      virtual const Orientation getDefaultOrientation() const = 0;

      /*! \return minimum width in pixels to use doubled size gui. */
      virtual const int getMinWidthToUseDoubleSizedGui() const = 0;

      /*! Do the specific app cycle (at each render cycle). */
      virtual void doCycle()=0;

      /*! \return true when defined its time to quit.
       * Usually, ignored by iOS and Android systems. */
      virtual bool shouldQuit()=0;
   
      /*! Update user input system's.
       * \return if should quit the application. */
      bool getInput();

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
      /*! Initialize the RTSS */
      bool initShaderSystem(Ogre::String userDir);
      
      Ogre::RTShader::ShaderGenerator* shaderGenerator; /**< RTSS */
      Goblin::MaterialListener* materialListener; /**< Material to shader */
#else 
      /*! Register the HLMS system */
      bool registerHLMS(Ogre::String hlmsPath);
#endif

      Ogre::Root* ogreRoot;                  /**< Ogre root */
      Ogre::RenderWindow* ogreWindow;        /**< Ogre Window */
      Ogre::SceneManager* ogreSceneManager;  /**< Ogre Scene Manager */

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
      Ogre::OverlaySystem* ogreOverlaySystem; /**< Ogre overlays */
#else
      Ogre::v1::OverlaySystem* ogreOverlaySystem; /**< Ogre overlays */
#endif
      
      Ogre::Timer updateTimer; /**< Timer to calculate frame interval */

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      Kobold::TouchInfo tInfo;               /**< Touch Info */
#else
      SDL_Window* sdlWindow;                 /**< SDL_Window used */
   #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      SDL_GLContext glContext;               /**< The OpenGL context */
   #endif
#endif
    
#ifdef _GOBLIN_SHOW_FPS_
      Goblin::FpsDisplay* fpsDisplay;        /**< The FPS displayer */
#endif

#if OGRE_VERSION_MAJOR != 1
      Ogre::CompositorWorkspace* ogreWorkspace; /**< Compositor workspace */
#endif

      int mouseX;                     /**< X position of single input */
      int mouseY;                     /**< Y position of single input */
      bool leftButtonPressed;         /**< If single input pressed */
      bool rightButtonPressed;        /**< If single input alternate pressed */
      bool backRunning;               /**< If app is background */ 
      bool receivedCameraInput; /**< If camera received input on last check */
      unsigned long timeElapsed; /**< Time elapsed before new call to render. */
};

}

#endif

