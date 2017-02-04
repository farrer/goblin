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

#include "baseapp.h"
#include "camera.h"
#include "screeninfo.h"
#include <kosound/sound.h>
#include <kobold/userinfo.h>
#include <kobold/i18n.h>
#include <kobold/log.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   #include <OGRE/RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#else
   #include <SDL_syswm.h>
   #include <iostream>
   #include <sstream>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   #include <OGRE/iOS/macUtils.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
   #include <OGRE/OSX/macUtils.h>
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #include "windows.h"
#else
   #include <time.h>
#endif


#if OGRE_VERSION_MAJOR != 1
   #include <OGRE/Compositor/OgreCompositorManager2.h>
   #include <OGRE/OgreArchive.h>
   #include <OGRE/OgreArchiveManager.h>
   #include <OGRE/OgreHlmsTextureManager.h>
#endif

using namespace Goblin;

/***********************************************************************
 *                              Constructor                            *
 ***********************************************************************/
BaseApp::BaseApp()
{
   dataPath = "";
   timeElapsed = 0;
   receivedCameraInput = false;
   backRunning = false;

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && \
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   sdlWindow = NULL;
#else
#endif

#ifdef _GOBLIN_SHOW_FPS_
   fpsDisplay = NULL;
#endif

   ogreRoot = NULL;
   ogreSceneManager = NULL;
   ogreWindow = NULL;
   ogreOverlaySystem = NULL;
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   shaderGenerator = NULL;
   materialListener = NULL;
#endif

   leftButtonPressed = false;
   mouseX = 0;
   mouseY = 0;

   /* Set a new seed */
   srand((unsigned int)time(NULL));
   updateTimer.reset();

   /* Init kobold log to use Ogre::Log */
   Kobold::Log::init(true);
}

/***********************************************************************
 *                               Destructor                            *
 ***********************************************************************/
BaseApp::~BaseApp()
{
   Kobold::Log::add("Finishing BaseApp...");
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS && \
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   Kobold::Log::add("   Finishing Kobold::Mouse...");
   Kobold::Mouse::finish();
#endif

#ifdef _GOBLIN_SHOW_FPS_
   if(fpsDisplay)
   {
      delete fpsDisplay;
   }
#endif

   Kobold::i18n::clear();
   
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   Kobold::MultiTouchController::finish();
#endif

   if(ogreOverlaySystem)
   {
      Kobold::Log::add("   Finishing ogreOverlaySystem...");
      delete ogreOverlaySystem;
   }

#if OGRE_VERSION_MAJOR != 1
   if(ogreWorkspace)
   {
      ogreWorkspace = NULL;
      Ogre::CompositorManager2* compositorManager = 
         ogreRoot->getCompositorManager2();
      if(compositorManager)
      {
         compositorManager->removeAllWorkspaces();
      }
   }
#endif

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   if(shaderGenerator)
   {
      Kobold::Log::add("   Finishing Shaders...");
      /* Restore default scheme. */
      Ogre::MaterialManager::getSingleton().setActiveScheme(
            Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
      /* Unregister the material manager listener. */
      if(materialListener != NULL)
      {
         Ogre::MaterialManager::getSingleton().removeListener(
               materialListener);
         delete materialListener;
      }
      shaderGenerator->destroy();
   }
#endif

   Kosound::Sound::finish();
   
   if(ogreRoot)
   {
      Kobold::Log::add("   Finishing Root...");
      delete ogreRoot;
   }

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   if(sdlWindow != NULL)
   {
      #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
         SDL_GL_DeleteContext(glContext);
      #endif
      SDL_DestroyWindow(sdlWindow);
      sdlWindow = NULL;
   }
   SDL_Quit();
#endif
}

/***********************************************************************
 *                          lowMemoryClean                             *
 ***********************************************************************/
void BaseApp::lowMemoryClean()
{
   doLowMemoryClean();
}

/***********************************************************************
 *                         sendToBackground                            *
 ***********************************************************************/
void BaseApp::sendToBackground()
{
   backRunning = true;
   doSendToBackground();
}

/***********************************************************************
 *                         sendToForeground                            *
 ***********************************************************************/
void BaseApp::sendToForeground()
{
   backRunning = false;
   doSendToForeground();
}

/***********************************************************************
 *                              getWindow                              *
 ***********************************************************************/
Ogre::RenderWindow* BaseApp::getWindow()
{
   return ogreWindow;
}

/***********************************************************************
 *                          getSceneManager                            *
 ***********************************************************************/
Ogre::SceneManager* BaseApp::getSceneManager()
{
   return ogreSceneManager;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID

/***********************************************************************
 *                         initAssetManager                            *
 ***********************************************************************/
void BaseApp::initAssetManager(JNIEnv* env, jobject assetManager, 
      jstring userHome, jstring cacheDir)
{
   AAssetManager* assetMgr = AAssetManager_fromJava(env, assetManager);
   if(assetMgr)
   {
      Ogre::APKFileSystemArchiveFactory* apkFileSystemArchiveFactory =
         new Ogre::APKFileSystemArchiveFactory(assetMgr);
      Ogre::ArchiveManager::getSingleton().addArchiveFactory(
            apkFileSystemArchiveFactory);
      Ogre::ArchiveManager::getSingleton().addArchiveFactory(
            new Ogre::APKZipArchiveFactory(assetMgr));
   }
   else
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: Couldn't retrieve the asset manager from java!");
   }

   Kobold::UserInfo::setAndroidDirectories(env, userHome, cacheDir);
}

/***********************************************************************
 *                         initWindowForAndroid                        *
 ***********************************************************************/
void BaseApp::initWindowForAndroid(JNIEnv* env, jobject surface)
{
   if(!surface)
   {
      return;
   }
   ANativeWindow* nativeWnd = ANativeWindow_fromSurface(env, surface);

   if((!nativeWnd) || (!ogreRoot))
   {
      return;
   }

   if(!ogreWindow)
   {
      Ogre::NameValuePairList opts;
      opts["externalWindowHandle"] = Ogre::StringConverter::toString(
            reinterpret_cast<size_t>(nativeWnd));
      opts[Ogre::String("Vsync")] = Ogre::String("No");
      opts[Ogre::String("FSAA")] = Ogre::String("4");

      ogreWindow = Ogre::Root::getSingleton().createRenderWindow(
            getApplicationUnixName(), 0, 0, false, &opts);
      create();
   }
   else
   {
      Ogre::GLRenderSystemCommon::_createInternalResources(ogreWindow, 
            nativeWnd, NULL);
   }
}

/***********************************************************************
 *                           termWindowForAndroid                      *
 ***********************************************************************/
void BaseApp::termWindowForAndroid()
{
   if((ogreRoot) && (ogreWindow))
   {
      Ogre::GLRenderSystemCommon::_destroyInternalResources(ogreWindow);
   }
}

/***********************************************************************
 *                       renderOneFrameForAndroid                      *
 ***********************************************************************/
void BaseApp::renderOneFrameForAndroid(JNIEnv* env, JavaVM *vm)
{  
   if(ogreWindow && ogreWindow->isActive())
   {  
      try
      {  
         if(vm->AttachCurrentThread(&env, NULL) < 0)
         {  
            return;
         }

         run();

         ogreWindow->windowMovedOrResized();
         ogreRoot->renderOneFrame();

         //gVM->DetachCurrentThread();
      }  
      catch(Ogre::RenderingAPIException& ex)
      {
      }
   }
}

#endif

/***********************************************************************
 *                             createRoot                              *
 ***********************************************************************/
bool BaseApp::createRoot()
{
   Ogre::RenderSystem* rs;

   /* Create the root, without cfg files */
   ogreRoot = new Ogre::Root("", "");

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
      OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   rs = new Ogre::GLES2RenderSystem();
   ogreRoot->addRenderSystem(rs);
#elif OGRE_VERSION_MAJOR == 1 || \
     (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   ogreRoot->loadPlugin(OGRE_GL_RENDER_SYSTEM_LIB);
   rs = ogreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
#else
   ogreRoot->loadPlugin(OGRE_GL3PLUS_RENDER_SYSTEM_LIB);
   rs = ogreRoot->getRenderSystemByName("OpenGL 3+ Rendering Subsystem");
#endif

   if(!rs)
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "FATAL: Couldn't define render system!");
      return false;
   }
   ogreRoot->setRenderSystem(rs);
   
   /* Init ogre, without loading from ogre.cfg */
   ogreRoot->initialise(false);

   return true;
}

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
/***********************************************************************
 *                              initRTSS                               *
 ***********************************************************************/
bool BaseApp::initShaderSystem(Ogre::String cacheDir)
{
   Kobold::Log::add(Kobold::Log::LOG_LEVEL_NORMAL, "Initing shader system...");
   if(Ogre::RTShader::ShaderGenerator::initialize())
   {
      shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

      /* Set the scene manager. */
      shaderGenerator->addSceneManager(ogreSceneManager);
 
      /* Set shader cache path.*/
      shaderGenerator->setShaderCachePath(cacheDir);

      /* create the material listener */
      materialListener = new Goblin::MaterialListener(shaderGenerator);
      Ogre::MaterialManager::getSingleton().addListener(materialListener);
     
      return true;
   }
   
   Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR, 
         "Error: Couldn't init shader system!");
   return false;
}
#else
/***********************************************************************
 *                         registerHLMS                                *
 ***********************************************************************/
bool BaseApp::registerHLMS(Ogre::String hlmsPath)
{
   Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingleton().getHlmsManager();

   Ogre::Archive* archiveLibrary = 
      Ogre::ArchiveManager::getSingletonPtr()->load(
         hlmsPath + "common/glsl", "FileSystem", true );

   Ogre::ArchiveVec library;
   library.push_back(archiveLibrary);

   Ogre::Archive* archiveUnlit = Ogre::ArchiveManager::getSingletonPtr()->load(
         hlmsPath + "unlit/glsl", "FileSystem", true );

   Ogre::HlmsUnlit* hlmsUnlit = new Ogre::HlmsUnlit(archiveUnlit, &library);
   hlmsManager->registerHlms(hlmsUnlit);

   Ogre::Archive* archivePbs = Ogre::ArchiveManager::getSingletonPtr()->load(
         hlmsPath + "pbs/glsl", "FileSystem", true );

   Ogre::HlmsPbs* hlmsPbs = new Ogre::HlmsPbs(archivePbs, &library);
   hlmsManager->registerHlms(hlmsPbs);

   /* If not using dds textures, should tell the manager  */
   if(!shouldUseBC5ForNormalTextures())
   {
      hlmsManager->getTextureManager()->getDefaultTextureParameters()[
          Ogre::HlmsTextureManager::TEXTURE_TYPE_NORMALS].pixelFormat = 
             Ogre::PF_R8G8_SNORM;
   }

   /* TODO: check if we need to restrict texture size (the examples from
    * Ogre only do it for DX11). */

   return true;
}
#endif

#if OGRE_VERSION_MAJOR == 1
/***********************************************************************
 *                           getShadowTechinque                        *
 ***********************************************************************/
Ogre::ShadowTechnique BaseApp::getShadowTechnique()
{
   return Ogre::SHADOWTYPE_NONE;
}
#endif

/***********************************************************************
 *                             getDataPath                             *
 ***********************************************************************/
Ogre::String BaseApp::getDataPath()
{
   return dataPath;
}

/***********************************************************************
 *                            defineDataPth                            *
 ***********************************************************************/
void BaseApp::defineDataPath()
{
   Ogre::String baseDataDir = getBaseDataDir();
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE ||\
    OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   dataPath = Ogre::macBundlePath() + Ogre::String("/") + baseDataDir +
                       Ogre::String("/");
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   dataPath = Ogre::String("/") + baseDataDir + Ogre::String("/");
#else
   //TODO: when installed somewhare, we must use it (ie: 
   // /usr/local/share, for example
   dataPath = baseDataDir + Ogre::String("/");
#endif
}

/***********************************************************************
 *                                 create                              *
 ***********************************************************************/
bool BaseApp::create(Ogre::String userHome, Ogre::uint32 wX,
                     Ogre::uint32 wZ, Ogre::Real wScale)
{
   /* Define application data path */
   defineDataPath();
   Ogre::String path = getDataPath();

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   /* Initialize SDL */
   if(SDL_Init(SDL_INIT_VIDEO) < 0)
   {
      return false;
   }

   #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
      SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
      SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
   #endif

   /* Create window from SDL */
   sdlWindow = SDL_CreateWindow(getApplicationUnixName().c_str(), 
         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
         wX, wZ, SDL_WINDOW_OPENGL);
   //TODO: must check if the window was created, otherwise, must try again
   //with less samples or without stencil buffer. 
   //TODO: fullscreen.
   #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      glContext = SDL_GL_CreateContext(sdlWindow);
   #endif

   /* Retrieve info from SDL */
   SDL_SysWMinfo wmInfo;
   SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
#endif

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   if(!createRoot())
   {
      return false;
   }
#endif
   
   /* Initialize user values and system specific configurations */
   Kobold::UserInfo::getValuesFromSystem(getApplicationUnixName(), userHome);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   
   Ogre::NameValuePairList opts;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   opts[Ogre::String("Display Frequency")] = Ogre::String("N/A");
   opts[Ogre::String("macAPI")] = Ogre::String("carbon");
   opts[Ogre::String("FSAA")] = Ogre::String("2");
#else
   opts[Ogre::String("Display Frequency")] = Ogre::String("50 MHz");
   opts[Ogre::String("FSAA")] = Ogre::String("4");
#endif
   opts[Ogre::String("RTT Preferred Mode")] = Ogre::String("FBO");
   opts[Ogre::String("Vsync")] = Ogre::String("No");

#if OGRE_VERSION_MAJOR > 2 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR > 0)
   opts[Ogre::String("gamma")] = Ogre::String("true");
#endif

   /* Create the window */
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
   ogreWindow = ogreRoot->createRenderWindow(getApplicationUnixName(), 
         wX, wZ, false, &opts);
#else
   /* Define window from the SDL's already created one */
   #if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      /*opts["parentWindowHandle"] = Ogre::StringConverter::toString(
            size_t(wmInfo.info.x11.window));*/
      opts["currentGLContext"] = Ogre::String("True");
   #elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      opts["externalWindowHandle"] = Ogre::StringConverter::toString(
            size_t(wmInfo.info.win.window));
   #elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
      opts["externalWindowHandle"] = Ogre::StringConverter::toString(
            size_t(wmInfo.info.cocoa.window));
   #endif
   ogreWindow = ogreRoot->createRenderWindow(getApplicationUnixName(), wX, wZ, 
            false, &opts);
#endif

#endif

   /* Create the scene manager */
#if OGRE_VERSION_MAJOR == 1
   //FIXME: we should do some check if need to use another scene manager.
   ogreSceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC);

   /* Set shadow technique to desired one (defaults to no shadows) */
   ogreSceneManager->setShadowTechnique(getShadowTechnique());
#else
   /* define num threads to use */
   const size_t numThreads = std::max<size_t>(1, 
         Ogre::PlatformInformation::getNumLogicalCores());

   Ogre::InstancingThreadedCullingMethod threadedCullingMethod = 
      Ogre::INSTANCING_CULLING_SINGLETHREAD;

   if(numThreads > 1)
   {
      /* Should use the threaded culling */
      threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
   }

   ogreSceneManager = ogreRoot->createSceneManager(Ogre::ST_GENERIC, 
         numThreads, threadedCullingMethod);

   /* Set sane defaults for proper shadow mapping */
   ogreSceneManager->setShadowDirectionalLightExtrusionDistance(500.0f);
   ogreSceneManager->setShadowFarDistance(500.0f);  
#endif
   
   /* Initialize the sound system */
   Kosound::Sound::init();

   /* Set the screen information. */
   Goblin::ScreenInfo::init(ogreWindow, wScale, 
         getMinWidthToUseDoubleSizedGui());

   /* Create the ogre overlay system */
   //FIXME: make overlay optional!
#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   ogreOverlaySystem = new Ogre::OverlaySystem();
#else
   ogreOverlaySystem = new Ogre::v1::OverlaySystem();
#endif
   ogreSceneManager->addRenderQueueListener(ogreOverlaySystem); 

   /* Create input */
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   Kobold::MultiTouchController::init(wScale);
   if(getDefaultOrientation() == LANDSCAPE)
   {
      Kobold::MultiTouchController::setOrientation(
            Kobold::ORIENTATION_DEGREE_0, wZ, wX);
   }
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    Ogre::String type = "APKFileSystem";
#else
    Ogre::String type = "FileSystem";
#endif

   /* Add data directories to the resource manager */
   Ogre::String* dataDirectories;
   Ogre::String* dataGroups;
   int totalDirectories = 0;
   getDataDirectories(&dataDirectories, &dataGroups, totalDirectories);

   for(int dir = 0; dir < totalDirectories; dir++)
   {
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
         path + dataDirectories[dir], type, dataGroups[dir], true);
   }
   /* Add user directory. */
   Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
         Kobold::UserInfo::getSaveDirectory(), "FileSystem", "saves", true);

   /* Add Cache temporary dir */
   Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
         Kobold::UserInfo::getCacheDirectory(), "FileSystem", "cache", false);


#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   Ogre::String shaderDir = "shaders/glsl";
#else
   Ogre::String shaderDir = "shaders/glsles";
#endif
//TODO: Win32 directx!

   /* Add shader dirs */
   Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
         path + shaderDir, type);

#if OGRE_VERSION_MAJOR == 1 || \
    (OGRE_VERSION_MAJOR == 2 && OGRE_VERSION_MINOR == 0)
   /* Initialize the shader system */
   if(!initShaderSystem(Kobold::UserInfo::getCacheDirectory()))
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: Couldn't init shader System!");
      return false;
   }
#else
   /* Let's setup HLMS to use */
   //FIXME: when using mobile (metal), if we'll support.
   if(!registerHLMS(path + "hlms/"))
   {
      Kobold::Log::add(Kobold::Log::LOG_LEVEL_ERROR,
            "Error: Couldn't register HLMS to use!");
      return false;
   }
#endif
 
   /* Define the camera */
   Goblin::Camera::init(ogreSceneManager, ogreWindow);
   
   /* Load things from resource. */
   Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

   /* Init the compositor system */
#if OGRE_VERSION_MAJOR != 1
   if(shouldCreateBasicWorkspace())
   {
      /* Create a the basic default workspace */
      Ogre::CompositorManager2 *compositorManager = 
         ogreRoot->getCompositorManager2();
      const Ogre::String workspaceName("Goblin Workspace");
      if(!compositorManager->hasWorkspaceDefinition(workspaceName))
      {
         compositorManager->createBasicWorkspaceDef(workspaceName, 
               Ogre::ColourValue(0.0f, 0.0f, 0.0f), Ogre::IdString());
      }
      ogreWorkspace = compositorManager->addWorkspace(ogreSceneManager,
            ogreWindow, Goblin::Camera::getOgreCamera(), workspaceName, true);
   }
   else
   {
      ogreWorkspace = NULL;
   }
#endif

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
   /* Initialize mouse cursor cursor */
   Kobold::Mouse::init();
#endif

   if(shouldUseKoboldI18n())
   {
      /* Load i18 files */
      Kobold::i18n::init("interface.def", Kobold::UserInfo::getLanguage());
   }

#ifdef _GOBLIN_SHOW_FPS_
   fpsDisplay = new Goblin::FpsDisplay(guiScore.getOverlay(), ogreWindow,
         "infoFont");
   fpsDisplay->setType(Goblin::FpsDisplay::TYPE_LAST);
   fpsDisplay->setPosition(0, 728);
#endif

   /* Do the cycle initialization */
   int callCounter = 0;
   bool shouldAbort = false;
   renderFrame();
   while((!shouldAbort) && (!doCycleInit(callCounter, shouldAbort)))
   {
      renderFrame();
      callCounter++;
   }
   return !shouldAbort;
}

/***********************************************************************
 *                            renderFrame                              *
 ***********************************************************************/
void BaseApp::renderFrame()
{
   /* Render the frame and update the window */
   ogreRoot->renderOneFrame();
#if OGRE_VERSION_MAJOR == 1
   ogreWindow->update();
#else
   ogreWindow->swapBuffers();
#endif
}

/***********************************************************************
 *                             getInput                                *
 ***********************************************************************/
bool BaseApp::getInput()
{
   bool shouldDoCameraInput = true;
   bool quit = false;

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   /* Clear the multitouch state */
   Kobold::MultiTouchController::update();

   /* Verify touch single input (as mouse) */
   rightButtonPressed = false; //TODO: alternate pressing for multitouch
   if(Kobold::MultiTouchController::totalTouches() >= 1)
   {
      Kobold::MultiTouchController::getTouch(0, tInfo);
      mouseX = tInfo.x;
      mouseY = tInfo.y;
      if( (tInfo.state == TOUCH_PRESSING) || 
            (tInfo.state == TOUCH_MOVED) )
      {
         leftButtonPressed = true;
      }
      else if(tInfo.state == TOUCH_RELEASED)
      {
         leftButtonPressed = false;
      }
      else
      {
         mouseX = -100;
         mouseY = -100;
         leftButtonPressed = false;
      }
   }
   else
   {
      mouseX = -100;
      mouseY = -100;
      leftButtonPressed = false;
   }
#else
   SDL_PumpEvents();

   /* Get Keyboard State */
   Kobold::Keyboard::updateState();
   Kobold::Mouse::update();
   /* Set mouse coordinates */
   leftButtonPressed = Kobold::Mouse::isLeftButtonPressed();
   rightButtonPressed = Kobold::Mouse::isRightButtonPressed();
   mouseX = Kobold::Mouse::getX();
   mouseY = Kobold::Mouse::getY();

   /* Let's update things by events (usually, only used for text editing and
    * mouse release states) */
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
      Kobold::Mouse::updateByEvent(event);
      if(Kobold::Keyboard::isEditingText())
      {
         Kobold::Keyboard::updateByEvent(event);
      }
      /* Let's check quit event */
      if(event.type == SDL_QUIT)
      {
         quit = true;
      }
   }
   shouldDoCameraInput = !Kobold::Keyboard::isEditingText();

#endif

   if(shouldDoCameraInput)
   {
      /* Do the camera input (if defined) */
      receivedCameraInput = Goblin::Camera::doMove();
   }

   return quit;
}

/***********************************************************************
 *                                  run                                *
 ***********************************************************************/
void BaseApp::run()
{  
   bool exit = false;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS ||\
    OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
   if(backRunning)
   {
      /* No need to do anything, as the app is in the background. */
      return;
   }
   timeElapsed = updateTimer.getMilliseconds();
#else
   if(!create("", getDefaultWindowWidth(), getDefaultWindowHeight()))
   {
      return;
   }
   
   /* The main loop (note: for IOS, only a single run, as the nstimer
    *  takes control. Android, similar case.) */
   while(!exit)
   {
      timeElapsed = updateTimer.getMilliseconds();
      if(timeElapsed > BASE_APP_UPDATE_RATE)
      {
#endif
         updateTimer.reset();
         
         /* Get input from multitouch, mouse or keyboard. */
         exit |= getInput();

#ifdef _GOBLIN_SHOW_FPS_
         fpsDisplay->update();
#endif

         /* Do the specific app cycle */
         doCycle();

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
         exit |= shouldQuit();
#endif
         /* Render the frame and update the window */
         renderFrame();

         /* Reset the 'listener' position to current camera */
         Kosound::Sound::setListenerPosition(Goblin::Camera::getCenterX(),
               Goblin::Camera::getCenterY(), Goblin::Camera::getCenterZ(),
               Goblin::Camera::getTheta(), Goblin::Camera::getPhi(), 
               Goblin::Camera::getZoom());
         
         /* Flush the sounds and music */
         Kosound::Sound::flush();
         
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS &&\
    OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
      }
      else if((BASE_APP_UPDATE_RATE - 1) - ((long)timeElapsed) > 0 )
      {
         Kobold::timer_Delay((BASE_APP_UPDATE_RATE - 1) - ((long)timeElapsed));
      }
      timeElapsed = updateTimer.getMilliseconds();
   }
#endif   
}



