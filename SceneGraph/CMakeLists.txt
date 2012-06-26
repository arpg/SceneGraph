project( SceneGraph )
cmake_minimum_required( VERSION 2.8 )

# scoop up our macros
include( Common.cmake )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

find_package( OpenGL REQUIRED )
find_package( ASSIMP REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( GLEW REQUIRED )
find_package( DevIL REQUIRED )

set( SceneGraph_SOURCES
        GLHelpers.cpp
        GLObject.cpp
        GLSceneGraph.cpp
	FBO.cpp
#	SimCam.cpp
   )

set( SceneGraph_HEADERS
        GLSLHelpers.h
        GLHelpers.h
        GLColor.h
        GLImage.h
        GLMesh.h
        GLText.h
        GLGrid.h
        GLAxis.h
#       SimCam.h
   )

# set for "export_library", below 
set( SceneGraph_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    	${OPENGL_INCLUDE_DIR}
        ${GLEW_INCLUDE_DIR}
        ${ASSIMP_INCLUDE_DIR}
        ${EIGEN3_INCLUDE_DIR}
        ${IL_INCLUDE_DIR}
   )

# set for "export_library", below 
set( SceneGraph_LIBRARIES
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        ${IL_LIBRARIES}
        ${ILU_LIBRARIES}
        ${ILUT_LIBRARIES}
   )

include_directories( ${SceneGraph_INCLUDE_DIRS} )
add_library( SceneGraph ${SceneGraph_SOURCES} ${SceneGraph_HEADERS} )
target_link_libraries( SceneGraph ${SceneGraph_LIBRARIES} )

# If set, this will use global variables SceneGraph_INCLUDE_DIRS and SceneGraph_LIBRARIES 
# and will write a SceneGraphConfig.cmake file with these values filled in.
export_library( SceneGraph )

