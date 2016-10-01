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

package org.dnteam.goblin;

import org.dnteam.goblin.ActivityJNI;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.Bundle;
import android.os.Handler;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.MotionEvent;
import android.content.res.AssetManager;
import android.util.Log;
import android.util.Pair;
import java.util.HashMap;

/** Main Activity. Heavly based on OGREJNI example. */
public abstract class MainJNIActivity extends Activity 
                                      implements SensorEventListener 
{
	protected Handler handler = null;
	protected SurfaceView surfaceView = null;
	protected Surface lastSurface = null;

	private Runnable renderer = null;
	private boolean paused = false;
	private boolean inited = false;
	private AssetManager assetMgr = null;

   /*! Last positions (Pair<x, y>) for each pointer id.
    * This is necessary, as Goblin keep each finger by its last position. */
   private HashMap<Integer, Pair<Integer, Integer>> lastPointerPos =
      new HashMap<Integer, Pair<Integer, Integer>>(); 
	
	@Override
	public void onCreate(Bundle savedInstanceState) 
   {
		super.onCreate(savedInstanceState);
		handler = new Handler();
		sysInit();
	}

	@Override
	protected void onPause() 
   {
		super.onPause();
		handler.removeCallbacks(renderer);
		paused = true;
	}

	@Override
	protected void onResume() 
   {
		super.onResume();
		paused = false;
		handler.post(renderer);
	}
	
	@Override
	protected void onDestroy() 
   {
		super.onDestroy();

		Runnable destroyer = new Runnable() 
      {
			public void run() 
         {
				ActivityJNI.destroy();
			}
		};
		handler.post(destroyer);
	}

	private void sysInit() 
   {
		final Runnable initRunnable = new Runnable() 
      {
			public void run() 
         {
				if (!inited) 
            {
					inited = true;
					
					if(assetMgr == null) {
						assetMgr = getResources().getAssets();
					}

               /*try {
                  String[] list = assetMgr.list("data");
                  for(String s : list) {
                     Log.i("asset", s);
                  }
               } catch(Exception e) {
                     Log.e("sysInit", "NO DATA!!!!");
               }*/
				
					ActivityJNI.create(assetMgr, 
                     getFilesDir().getAbsolutePath(),
                     getCacheDir().getAbsolutePath());

					renderer = new Runnable() 
               {
						public void run() 
                  {

							if (paused)
								return;

							if (!wndCreate && lastSurface != null) 
                     {
								wndCreate = true;
								ActivityJNI.initWindow(lastSurface);
								handler.post(this);
								return;
							}

							if (inited && wndCreate)
								ActivityJNI.renderOneFrame();

							handler.post(this);
						}
					};

					handler.post(renderer);
				}
			}

		};

		SurfaceView view = new SurfaceView(this);
		SurfaceHolder holder = view.getHolder();
		// holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
		surfaceView = view;

		holder.addCallback(new Callback() 
      {
			public void surfaceCreated(SurfaceHolder holder) 
         {
				if (holder.getSurface() != null
						&& holder.getSurface().isValid()) 
            {
					lastSurface = holder.getSurface();
					handler.post(initRunnable);
				}
			}

			public void surfaceDestroyed(SurfaceHolder holder) 
         {
				if (inited && wndCreate) 
            {
					wndCreate = false;
					lastSurface = null;
					handler.post(new Runnable() 
               {
						public void run() 
                  {
							ActivityJNI.termWindow();
						}
					});
				}
			}

			public void surfaceChanged(SurfaceHolder holder, int format,
					int width, int height) 
         {

			}
		});
		setContentView(surfaceView);

      view.setOnTouchListener(new OnTouchListener() 
      {
         @Override
         public boolean onTouch(View v, MotionEvent event) 
         {

            switch(event.getActionMasked())
            {
               /* The first finger will send a ACTION_DOWN, with more than one,
                * the other will send a POINTER_DOWN action. */
               case MotionEvent.ACTION_DOWN:
               case MotionEvent.ACTION_POINTER_DOWN:
               {
                  /* Define lastPos for new pointer  */
                  Integer pointerId = event.getPointerId(
                        event.getActionIndex());
                  Pair<Integer, Integer> pos = new Pair<Integer, Integer>(
                        Math.round(event.getX()), Math.round(event.getY()));
                  lastPointerPos.put(pointerId, pos);
                  /* Tell about the finger down */
                  ActivityJNI.touchBegan(pos.first, pos.second);
               }
               break;
               /* The UP is similar: when have more than one finger active,
                * will receive POINTER_UP. With just a finger, will receive
                * ACTION_UP action. */
               case MotionEvent.ACTION_UP:
               case MotionEvent.ACTION_POINTER_UP:
               {
                  /* Get last pos */
                  Integer pointerId = event.getPointerId(
                        event.getActionIndex());
                  Pair<Integer, Integer> lastPos = lastPointerPos.get(
                        pointerId);
                  /* Define new pos, tell finger up */
                  Pair<Integer, Integer> pos = new Pair<Integer, Integer>(
                        Math.round(event.getX()), Math.round(event.getY()));
                  ActivityJNI.touchEnded(lastPos.first, lastPos.second,
                        pos.first, pos.second);
                  /* Remove from the hash */
                  lastPointerPos.remove(pointerId);
               }
               break;
               case MotionEvent.ACTION_MOVE:
               {
                  final int historySize = event.getHistorySize();
                  final int pointerCount = event.getPointerCount();
                  for(int h = 0; h < historySize; h++) 
                  {
                     for (int p = 0; p < pointerCount; p++) 
                     {
                        /* Get last post */
                        Integer pointerId = event.getPointerId(p);
                        Pair<Integer, Integer> lastPos = lastPointerPos.get(
                              pointerId);
                        /* Get current historical pos */
                        Pair<Integer, Integer> pos = new Pair<Integer, Integer>(
                              Math.round(event.getHistoricalX(p, h)), 
                              Math.round(event.getHistoricalY(p, h)));
                        /* Tell finger moved */
                        ActivityJNI.touchMoved(lastPos.first, 
                              lastPos.second, pos.first, pos.second);
                        /* Put current pos as last. */
                        lastPointerPos.put(pointerId, pos);
                     }
                  }
                  for (int p = 0; p < pointerCount; p++) 
                  {

                     /* Get last post */
                     Integer pointerId = event.getPointerId(p);
                     Pair<Integer, Integer> lastPos = lastPointerPos.get(
                           pointerId);
                     /* Get current historical pos */
                     Pair<Integer, Integer> pos = new Pair<Integer, Integer>(
                           Math.round(event.getX(p)), 
                           Math.round(event.getY(p)));
                     /* Tell finger moved */
                     ActivityJNI.touchMoved(lastPos.first, 
                           lastPos.second, pos.first, pos.second);
                     /* Put current pos as last. */
                     lastPointerPos.put(pointerId, pos);
                  }
               }
               break;
               case MotionEvent.ACTION_CANCEL:
               {
                  /* Get last pos */
                  Integer pointerId = event.getPointerId(
                        event.getActionIndex());
                  Pair<Integer, Integer> lastPos = lastPointerPos.get(
                        pointerId);
                  /* Tell touch cancelled */
                  Pair<Integer, Integer> pos = new Pair<Integer, Integer>(
                        Math.round(event.getX()), Math.round(event.getY()));
                  ActivityJNI.touchCanceled(lastPos.first, lastPos.second);
                  /* Remove from the hash */
                  lastPointerPos.remove(pointerId);
               }
               break;
               default:
               return false;
            }
            return true;
         }
      });
	}

	boolean wndCreate = false;

	public void onAccuracyChanged(Sensor sensor, int accuracy) 
   {

	}

	public void onSensorChanged(SensorEvent event) 
   {
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) 
      {
		}
	}

	static {
	   System.loadLibrary("openal");
		System.loadLibrary("GoblinJNI");
   }
}
