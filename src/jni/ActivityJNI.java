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

import android.view.Surface;
import android.content.res.AssetManager;

public class ActivityJNI 
{
 
   /* Functions related to context creation / render / destroy. */
	public native static void create(AssetManager mgr, String userHome, 
                                    String cacheDir);	
	public native static void destroy();	
	public native static void initWindow(Surface surface);
	public native static void termWindow();
	public native static void renderOneFrame();

   /* Functions related to touch input. */
   public native static void touchBegan(int x, int y);
   public native static void touchMoved(int prevX, int prevY, int x, int y);
   public native static void touchEnded(int prevX, int prevY, int x, int y);
   public native static void touchCanceled(int x, int y);
}
