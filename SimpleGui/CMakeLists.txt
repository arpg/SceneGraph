project( SimpleGui )
cmake_minimum_required( VERSION 2.8 )

# scoop up our macros
include( Common.cmake )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

find_package( Boost COMPONENTS thread REQUIRED)
find_package( FLTK REQUIRED )
find_package( CVARS REQUIRED )
find_package( OpenGL REQUIRED )
find_package( ASSIMP REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( GLEW REQUIRED )
#find_package( GLUT REQUIRED )

set( SimpleGui_SOURCES
        GLHelpers.cpp
        GLWindow.cpp
        GLObject.cpp
        GLSceneGraph.cpp
   )

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
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${BOOST_INCLUDE_DIR}
        ${FLTK_INCLUDE_DIR}
        ${CVARS_INCLUDE_DIR}
    	${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
#        ${GLUT_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${EIGEN3_INCLUDE_DIR}
   )

# set for "export_library", below 
set( SimpleGui_LIBRARIES
        ${Boost_THREAD_LIBRARY}
        ${FLTK_LIBRARIES}
        ${CVARS_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
#        ${GLUT_LIBRARIES}
        ${ASSIMP_LIBRARIES}
   )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_library( SimpleGui ${SimpleGui_SOURCES} ${SimpleGui_HEADERS} )
target_link_libraries( SimpleGui ${SimpleGui_LIBRARIES} )

# If set, this will use global variables SimpleGui_INCLUDE_DIRS and SimpleGui_LIBRARIES 
# and will write a SimpleGuiConfig.cmake file with these values filled in.
export_library( SimpleGui )

