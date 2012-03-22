project( SimpleGui )
cmake_minimum_required(VERSION 2.8)

# scoop up our macros
include( Common.cmake )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

find_package( Boost COMPONENTS thread REQUIRED)
find_package( FLTK REQUIRED )
find_package( CVARS REQUIRED )
find_package( OpenGL REQUIRED )
find_package( ASSIMP REQUIRED )

set( SimpleGui_HEADERS
        GLWindow.h
        GLHelpers.h
        GLColor.h
        GLMesh.h
        GLText.h
        GLGrid.h
   )

# set for "export_library", below 
set( SimpleGui_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CVARS_INCLUDE_DIR} 
        ${FLTK_INCLUDE_DIR} 
        ${EIGEN3_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${BOOST_INCLUDE_DIR}
   )

# set for "export_library", below 
set( SimpleGui_LIBRARIES
        ${OPENGL_LIBRARIES} 
        ${FLTK_LIBRARIES} 
        ${CVARS_LIBRARIES} 
        ${ASSIMP_LIBRARIES}
        ${Boost_THREAD_LIBRARY}
   )

# If set, this will use global variables SimpleGui_INCLUDE_DIRS and SimpleGui_LIBRARIES 
# and will write a SimpleGuiConfig.cmake file with these values filled in.
export_library( SimpleGui )

