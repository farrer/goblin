# Goblin - An Ogre3D Utility Library

## About
Goblin is an Ogre3D library with usefull codes for this rendering engine used
by DNTeam applications.

## License
Goblin is released under GNU Lesser General Public License v3.0 or later
[https://www.gnu.org/licenses/lgpl.html]

## Dependencies

 * Ogre3D 1.10 [http://www.ogre3d.org/]
 * Kobold [https://github.com/farrer/kobold]
 * Kosound [https://github.com/farrer/kosound]
 * SDL2 [https://libsdl.org/download-2.0.php]
 * OpenAL [http://kcat.strangesoft.net/openal.html]
 * Ogg [https://xiph.org/downloads/]
 * Vorbis [https://xiph.org/downloads/]
 * VorbisFile [https://xiph.org/downloads/]

## Building

Usually, you build Goblin with the following commands:

**mkdir** build  
**cd** build  
cmake ..  
make  
make install

### Building for Android

To build for Android, the CMake command call must set the needed NDK variables:

cmake -DCMAKE\_TOOLCHAIN\_FILE=../CMakeModules/android.toolchain.cmake -DANDROID\_NDK=path\_to\_Android\_Ndk -DCMAKE\_BUILD\_TYPE=Release -DANDROID\_ABI="armeabi-v7a" -DANDROID\_NATIVE\_API\_LEVEL=12 ..

Change those parameters to your needs.


### Options

There are some options that could be passed to CMake script:

 * GOBLIN\_DEBUG -> Build the library with debugging symbols;
 * GOBLIN\_STATIC -> Build a .a static library, instead of the shared one.

