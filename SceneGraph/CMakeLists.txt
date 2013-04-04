project( SceneGraph )
cmake_minimum_required( VERSION 2.8 )

# scoop up our macros
include( Common.cmake )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

find_package( OpenGL REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( GLEW REQUIRED )

list(APPEND
    SceneGraph_SOURCES
    GLHelpers.cpp
    GLObject.cpp
    GLSceneGraph.cpp
    FBO.cpp
    SimCam.cpp
)

list(APPEND
    SceneGraph_HEADERS
    config.h.in
    SceneGraph.h
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
)

# Basic includes / libraries
set(SceneGraph_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/..;${PROJECT_BINARY_DIR}/..;${OPENGL_INCLUDE_DIR};${GLEW_INCLUDE_DIR};${EIGEN3_INCLUDE_DIR}" )
set(SceneGraph_LIBRARIES "${OPENGL_LIBRARIES};${GLEW_LIBRARIES}" )

find_package( ASSIMP )
if(ASSIMP_FOUND)
  set(HAVE_ASSIMP 1)
  list(APPEND SceneGraph_INCLUDE_DIRS "${ASSIMP_INCLUDE_DIRS}" )
  list(APPEND SceneGraph_LIBRARIES "${ASSIMP_LIBRARIES}" )
  list(APPEND SceneGraph_SOURCES GLMesh.cpp)
  list(APPEND SceneGraph_HEADERS GLMesh.h)
else()
  message(STATUS "ASSIMP Not found, but required for mesh loading.")
endif()

find_package( DevIL )
if(IL_FOUND)
  set(HAVE_DEVIL 1)
  list(APPEND SceneGraph_INCLUDE_DIRS "${IL_INCLUDE_DIR};${IL_INCLUDE_DIR}/.." )
  list(APPEND SceneGraph_LIBRARIES "${IL_LIBRARIES};${ILU_LIBRARIES};${ILUT_LIBRARIES}" )
else()
  message(STATUS "DevIL not found, but it is only used to load images that boost cannot load")
endif()

find_package(PNG)
if(PNG_FOUND)
  set(HAVE_PNG 1)
  list(APPEND SceneGraph_INCLUDE_DIRS ${PNG_INCLUDE_DIR} )
  list(APPEND SceneGraph_LIBRARIES ${PNG_LIBRARY} )
else()
  message(STATUS "libpng not found - used to load PNG textures")
endif()

find_package(JPEG)
if(JPEG_FOUND)
  set(HAVE_JPEG 1)
  list(APPEND SceneGraph_INCLUDE_DIRS ${JPEG_INCLUDE_DIR} )
  list(APPEND SceneGraph_LIBRARIES ${JPEG_LIBRARY} )
else()
  message(STATUS "libjpeg not found - used to load Jpeg textures")
endif()

find_package(TIFF)
if(TIFF_FOUND)
  set(HAVE_TIFF 1)
  list(APPEND SceneGraph_INCLUDE_DIRS ${TIFF_INCLUDE_DIR} )
  list(APPEND SceneGraph_LIBRARIES ${TIFF_LIBRARY} )
else()
  message(STATUS "libtiff not found - used to load TIFF textures")
endif()

# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin)
if(Pangolin_FOUND)
  set(HAVE_PANGOLIN 1)
  include_directories(${Pangolin_INCLUDE_DIRS})
  link_directories(${Pangolin_LIBRARY_DIRS})
  link_libraries(${Pangolin_LIBRARIES})
endif()

include_directories( ${SceneGraph_INCLUDE_DIRS} )
add_library( SceneGraph ${SceneGraph_SOURCES} ${SceneGraph_HEADERS} )
target_link_libraries( SceneGraph ${SceneGraph_LIBRARIES} )

# Test platform type
IF(WIN32 OR WIN64)
  SET(_WIN_ 1)
ENDIF()

IF(UNIX)
  SET(_UNIX_ 1)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   SET(_OSX_ 1)
ENDIF()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   SET(_LINUX_ 1)
ENDIF()

# Generate config file for use in headers
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

# If set, this will use global variables SceneGraph_INCLUDE_DIRS and SceneGraph_LIBRARIES
# and will write a SceneGraphConfig.cmake file with these values filled in.
export_library( SceneGraph )

