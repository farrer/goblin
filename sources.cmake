
########################################################################
# Files related to the goblin library
########################################################################
set(GOBLIN_SOURCES
src/baseapp.cpp
src/camera.cpp
src/cursor.cpp
src/fpsdisplay.cpp
src/guiobject.cpp
src/ibar.cpp
src/ibutton.cpp
src/image.cpp
src/model3d.cpp
src/materiallistener.cpp
src/screeninfo.cpp
src/textbox.cpp
src/texttitle.cpp
src/vertexutils.cpp
)

set(GOBLIN_HEADERS
src/baseapp.h
src/camera.h
src/cursor.h
src/fpsdisplay.h
src/guiobject.h
src/ibar.h
src/ibutton.h
src/image.h
src/model3d.h
src/materiallistener.h
src/screeninfo.h
src/textbox.h
src/texttitle.h
src/vertexutils.h
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
set(GOBLIN_SOURCES
   ${GOBLIN_SOURCES}
   src/jni/ActivityJNI.cpp
)
endif(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
 

