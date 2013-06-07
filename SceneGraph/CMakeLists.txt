project( SceneGraph )
cmake_minimum_required( VERSION 2.8 )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

################################################################################
# Find required dependencies


find_package( Eigen3 REQUIRED )


# Basic includes / libraries
set(USER_INC "${CMAKE_CURRENT_SOURCE_DIR}/..;${PROJECT_BINARY_DIR}/..;${EIGEN3_INCLUDE_DIR}" )
set(LINK_LIBS "" )
set(LIB_DIRS "" )

if(NOT _ANDROID_)
    find_package( OpenGL REQUIRED )
    find_package( GLEW REQUIRED )
    list(APPEND USER_INC  "${OPENGL_INCLUDE_DIR};${GLEW_INCLUDE_DIR}" )
    list(APPEND LINK_LIBS "${OPENGL_LIBRARIES};${GLEW_LIBRARY}" )
else()
    find_package(GLUES REQUIRED)
    list(APPEND LINK_LIBS "${GLUES_LIBRARY}" )
    list(APPEND LINK_LIBS "-lEGL;-lGLESv2;-lGLESv1_CM" )
    set(HAVE_GLES 1)
endif()


################################################################################
# Specify source files

list(APPEND SOURCES    
    GLSceneGraph.cpp
    GLObject.cpp
)

if( NOT _ANDROID_ )
list(APPEND HEADERS
    GLHelpers.cpp
    FBO.cpp
    SimCam.cpp
    ../Widgets/nvGlutWidgets.cpp
    ../Widgets/nvGLWidgets.cpp
    ../Widgets/nvWidgets.cpp
)
endif()

list(APPEND HEADERS
    config.h.in
    SceneGraph.h    
)

if( NOT _ANDROID_ )
list(APPEND HEADERS
    GLGroup.h
    GLSLHelpers.h
    GLCylinder.h
    GLHelpers.h
    GLHelpersLoadTextures.h
    GLHelpersBoost.h
    GLHelpersDevil.h
    GLLight.h
    GLShadowLight.h
    GLColor.h
    GLImage.h
    GLVbo.h
    GLText.h
    GLGrid.h
    GLAxis.h
    GLCube.h
    GLMovableAxis.h
    GLAxisAlignedBox.h
    GLWaypoint.h
    GLLineStrip.h
    GLPrimitives.h
    GLAxisHistory.h
    GLOpenBox.h
    SimCam.h
    Pangolin.h
    PangolinDrawGLObject.h
    PangolinSceneGraphHandler.h
    PangolinImageView.h
    ../Widgets/nvWidgets.h
    ../Widgets/nvGlutWidgets.h
    ../Widgets/nvGLWidgets.h
    ../Widgets/nvShaderUtils.h
    ../Widgets/GLWidgetView.h
)
endif()


################################################################################
# Find optional libraries

find_package( ASSIMP QUIET)
if(ASSIMP_FOUND)
  set(HAVE_ASSIMP 1)
  list(APPEND USER_INC "${ASSIMP_INCLUDE_DIR}" )
  list(APPEND LINK_LIBS "${ASSIMP_LIBRARIES}" )
  list(APPEND SOURCES GLMesh.cpp)
  list(APPEND HEADERS GLMesh.h)
else()
  message(STATUS "ASSIMP Not found, but required for mesh loading.")
endif()

find_package( DevIL QUIET)
if(IL_FOUND)
  set(HAVE_DEVIL 1)
  list(APPEND USER_INC "${IL_INCLUDE_DIR};${IL_INCLUDE_DIR}/.." )
  list(APPEND LINK_LIBS "${IL_LIBRARIES};${ILU_LIBRARIES};${ILUT_LIBRARIES}" )
else()
  message(STATUS "DevIL not found, but it is only used to load images that boost cannot load")
endif()

find_package(PNG QUIET)
if(PNG_FOUND)
  set(HAVE_PNG 1)
  list(APPEND USER_INC ${PNG_INCLUDE_DIR} )
  list(APPEND LINK_LIBS ${PNG_LIBRARY} )
else()
  message(STATUS "libpng not found - used to load PNG textures")
endif()

find_package(JPEG QUIET)
if(JPEG_FOUND)
  set(HAVE_JPEG 1)
  list(APPEND USER_INC ${JPEG_INCLUDE_DIR} )
  list(APPEND LINK_LIBS ${JPEG_LIBRARY} )
else()
  message(STATUS "libjpeg not found - used to load Jpeg textures")
endif()

find_package(TIFF QUIET)
if(TIFF_FOUND)
  set(HAVE_TIFF 1)
  list(APPEND USER_INC ${TIFF_INCLUDE_DIR} )
  list(APPEND LINK_LIBS ${TIFF_LIBRARY} )
else()
  message(STATUS "libtiff not found - used to load TIFF textures")
endif()

# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.1)
if(Pangolin_FOUND)
  set(HAVE_PANGOLIN 1)
  list(APPEND USER_INC ${Pangolin_INCLUDE_DIRS})
  list(APPEND LINK_LIBS ${Pangolin_LIBRARIES})
  list(APPEND LINK_DIRS ${Pangolin_LIBRARY_DIRS})
endif()

################################################################################
# Include collected includes / libraries

include_directories( ${USER_INC} )
add_library( SceneGraph ${SOURCES} ${HEADERS} )
target_link_libraries( SceneGraph ${LINK_LIBS} )

# Generate config file for use in headers
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

################################################################################
# Export library using cmake package mechanism.

# Create SceneGraphConfig.cmake from SceneGraphConfig.cmake.in
# to export USER_INC, LINK_LIBS etc.
get_target_property( LIB_LOC SceneGraph LOCATION )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/SceneGraphConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SceneGraphConfig.cmake @ONLY IMMEDIATE )

if(NOT _ANDROID_)
    # Add package to CMake package registery for use from the build tree
    export( PACKAGE SceneGraph )
endif()

