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

#include <jni.h>
#include <EGL/egl.h>
#include <android/api-level.h>
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <OGRE/OgrePlatform.h>

#include <kobold/multitouchcontroller.h>
#include "../baseapp.h"

using namespace Goblin;

/*! To use the Goblin JNI, one must implement the function bellow,
 * allocating in it the application to run here.
 * \return pointer to the created application (which is a child of BaseApp). */
Goblin::BaseApp* createApplication();

namespace 
{
   /*! The application to run. */
   Goblin::BaseApp* app = NULL;
   /*! The Virtual machine used. */
   JavaVM* gVM = NULL;
}

/* enable JNI calling conventions for functions defined here */
extern "C" 
{
   /* Functions related to context creation */
   JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);
   JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_create(
         JNIEnv* env, jobject obj, jobject assetManager, jstring userHome, 
         jstring cacheDir);
   JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_destroy(
         JNIEnv* env, jobject obj);
   JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_initWindow(
         JNIEnv* env, jobject obj,  jobject surface);
   JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_termWindow(
         JNIEnv* env, jobject obj);
   JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_renderOneFrame(
         JNIEnv* env, jobject obj);

    /* Functions related to multitouch input */
    JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_touchBegan(
          JNIEnv* env, jobject obj, int x, int y);
    JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_touchEnded(
          JNIEnv* env, jobject obj, int prevX, int prevY, int x, int y);
    JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_touchMoved(
          JNIEnv* env, jobject obj, int prevX, int prevY, int x, int y);
    JNIEXPORT void JNICALL Java_org_dnteam_goblin_ActivityJNI_touchCanceled(
          JNIEnv* env, jobject obj, int x, int y);
}

/**************************************************************************
 *                               JNI_OnLoad                               *
 **************************************************************************/
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
   gVM = vm;
   return JNI_VERSION_1_4;
}

/**************************************************************************
 *                            ActivityJNI_create                          *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_create(JNIEnv* env, jobject obj, 
      jobject assetManager, jstring userHome, jstring cacheDir)
{
   if(!app)
   {
      /* Create the application and its context. */
      app = createApplication();
      app->createRoot();
      app->initAssetManager(env, assetManager, userHome, cacheDir);
   }
}

/**************************************************************************
 *                           ActivityJNI_destroy                          *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_destroy(JNIEnv* env, jobject obj)
{
   if(app)
   {
      delete app;
      app = NULL;
   }
}

/**************************************************************************
 *                         ActivityJNI_initWindow                         *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_initWindow(JNIEnv* env, jobject obj, 
      jobject surface)
{
   if(app)
   {
      app->initWindowForAndroid(env, surface);
   }
}

/**************************************************************************
 *                          ActivityJNI_termWindow                        *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_termWindow(JNIEnv* env, jobject obj)
{
   if(app)
   {
      app->termWindowForAndroid();
   }
}

/**************************************************************************
 *                         ActivityJNI_renderOneFrame                     *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_renderOneFrame(JNIEnv* env, 
      jobject obj)
{
   if(app) 
   {
      app->renderOneFrameForAndroid(env, gVM);
   }
}

/**************************************************************************
 *                          ActivityJNI_touchBegan                        *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_touchBegan(JNIEnv* env, 
      jobject obj, int x, int y)
{
   Kobold::MultiTouchController::touchBegan(x, y);
}

/**************************************************************************
 *                          ActivityJNI_touchEnded                        *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_touchEnded(JNIEnv* env, 
      jobject obj, int prevX, int prevY, int x, int y)
{
   Kobold::MultiTouchController::touchEnded(prevX, prevY, x, y);
}

/**************************************************************************
 *                          ActivityJNI_touchMoved                        *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_touchMoved(JNIEnv* env, 
      jobject obj, int prevX, int prevY, int x, int y)
{
   Kobold::MultiTouchController::touchMoved(prevX, prevY, x, y);
}

/**************************************************************************
 *                        ActivityJNI_touchCanceled                       *
 **************************************************************************/
void Java_org_dnteam_goblin_ActivityJNI_touchCanceled(JNIEnv* env, 
      jobject obj, int x, int y)
{
   Kobold::MultiTouchController::touchCanceled(x, y);
}


