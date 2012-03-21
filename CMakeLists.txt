project( test )
cmake_minimum_required(VERSION 2.8)

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

#the boost libraries that we need
set(BOOST_LIBS thread)


find_package( Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_package( FLTK REQUIRED )
find_package( CVARS REQUIRED )
find_package( OpenGL REQUIRED )
find_package( OpenCV REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( ZeroMQ REQUIRED )
find_package( Protobuf REQUIRED )
find_package( ASSIMP REQUIRED )

#include_directories( . )
include_directories( ${BOOST_INCLUDE_DIR} )
include_directories( ${FLTK_INCLUDE_DIR} )
include_directories( ${CVARS_INCLUDE_DIR} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${EIGEN3_INCLUDE_DIR} )
include_directories( ${ZeroMQ_INCLUDE_DIRS} )
include_directories( ${ASSIMP_INCLUDE_DIR} )

set( hdrs   GLWindow.h
            GLHelpers.h
            GLColor.h
            GLMesh.h
            GLText.h
            GLGrid.h
            AssetLoader.h
   )

set( srcs   GLWindow.cpp )

add_executable( test ${srcs} ${hdrs} )
target_link_libraries( test ${Boost_THREAD_LIBRARY} )
target_link_libraries( test ${CVARS_LIBRARIES} )
target_link_libraries( test ${FLTK_LIBRARIES} )
target_link_libraries( test ${OPENGL_LIBRARIES} )
target_link_libraries( test ${OpenCV_LIBS} )
target_link_libraries( test ${ASSIMP_LIBRARIES} )

