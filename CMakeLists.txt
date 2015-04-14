project( SceneGraph )
cmake_minimum_required(VERSION 2.8)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})

# Add to module path, so we can find our cmake modules


list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )

message( STATUS "CMAKE_ROOT ${CMAKE_ROOT}")
message( STATUS "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}")

include( install_library )

################################################################################

option(BUILD_EXAMPLES "Build Examples" ON)

option(BUILD_SHARED_LIBS "Build Shared Library" ON)
set(ANDROID_PACKAGE_NAME "edu.colorado.arpg.scenegraph")

# Overide with cmake -DCMAKE_BUILD_TYPE=Debug {dir}
if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif()

if( NOT MSVC )
  # Set GCC style compiler flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -std=c++11")

  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif()

string( TOLOWER ${PROJECT_NAME} LIBRARY_NAME )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/SceneGraph/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/SceneGraph/SceneGraphConfig.h)

################################################################################
# Find required dependencies

find_package( Eigen3 REQUIRED )

# Basic includes / libraries
set(REQUIRED_LIBRARIES ${EIGEN3_LIBRARIES} )
set(REQUIRED_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIRS})

if(NOT _ANDROID_)
    find_package( OpenGL REQUIRED )
    find_package( GLEW REQUIRED )
    list(APPEND REQUIRED_INCLUDE_DIRS "${OPENGL_INCLUDE_DIR};${GLEW_INCLUDE_DIR}" )
    list(APPEND REQUIRED_LIBRARIES "${OPENGL_LIBRARIES};${GLEW_LIBRARY}" )
else()
    find_package(GLUES REQUIRED)
    list(APPEND REQUIRED_LIBRARIES "${GLUES_LIBRARY}" )
    list(APPEND REQUIRED_LIBRARIES "-lEGL;-lGLESv2;-lGLESv1_CM" )
    list(APPEND REQUIRED_INCLUDE_DIRS "${GLUES_INCLUDE_DIR}")
    set(HAVE_GLES 1)
    set(HAVE_GLUES 1)
endif()


################################################################################
# Specify source files

set( SCENEGRAPH_SRCS
    src/GLSceneGraph.cpp
    src/GLAxis.cpp
    src/GLObject.cpp
    src/GLLight.cpp
    src/GLCube.cpp
    src/GLGrid.cpp
    src/GLDynamicGrid.cpp
)

set( SCENEGRAPH_HDRS
    include/SceneGraph/config.h.in
    include/SceneGraph/SceneGraph.h
    include/SceneGraph/GLHelpers.h
    include/SceneGraph/GLSLHelpers.h
    include/SceneGraph/GLSceneGraph.h
    include/SceneGraph/GLHeightmap.h
    include/SceneGraph/GLObject.h
    include/SceneGraph/GLGroup.h
    include/SceneGraph/GLCylinder.h
    include/SceneGraph/GLLight.h
    include/SceneGraph/GLShadowLight.h
    include/SceneGraph/GLColor.h
    include/SceneGraph/GLImage.h
    include/SceneGraph/GLVbo.h
    include/SceneGraph/GLText.h
    include/SceneGraph/GLGrid.h
    include/SceneGraph/GLAxis.h
    include/SceneGraph/GLCube.h
    include/SceneGraph/GLTeapot.h
    include/SceneGraph/GLLineStrip.h
    include/SceneGraph/GLPrimitives.h
    include/SceneGraph/GLAxisHistory.h
    include/SceneGraph/GLOpenBox.h
    include/SceneGraph/GLHelpersLoadTextures.h
    include/SceneGraph/GLHelpersDevil.h
    include/SceneGraph/GLWireSphere.h
    include/SceneGraph/AxisAlignedBoundingBox.h
    include/SceneGraph/LineSegment.h
    include/SceneGraph/PangolinDrawGLObject.h
    include/SceneGraph/PangolinSceneGraphHandler.h
    include/SceneGraph/PangolinImageView.h
)

if( NOT ANDROID )
    list(APPEND SCENEGRAPH_SRCS
        src/GLHelpers.cpp
        src/FBO.cpp
        src/SimCam.cpp
        src/Widgets/nvGlutWidgets.cpp
        src/Widgets/nvGLWidgets.cpp
        src/Widgets/nvWidgets.cpp
    )

    list(APPEND SCENEGRAPH_HDRS
        include/SceneGraph/GLMovableAxis.h
        include/SceneGraph/GLAxisAlignedBox.h
        include/SceneGraph/GLWaypoint.h
        include/SceneGraph/FBO.h
        include/SceneGraph/SimCam.h
        include/SceneGraph/Widgets/nvWidgets.h
        include/SceneGraph/Widgets/nvGlutWidgets.h
        include/SceneGraph/Widgets/nvGLWidgets.h
        include/SceneGraph/Widgets/nvShaderUtils.h
        include/SceneGraph/Widgets/GLWidgetView.h
    )
endif()


################################################################################
# Find optional libraries

find_package( ASSIMP QUIET)
if(ASSIMP_FOUND)
  set(HAVE_ASSIMP 1)
  list(APPEND REQUIRED_INCLUDE_DIRS "${ASSIMP_INCLUDE_DIR}" )
  list(APPEND REQUIRED_LIBRARIES "${ASSIMP_LIBRARIES}" )
  list(APPEND SCENEGRAPH_SRCS src/GLMesh.cpp)
  list(APPEND SCENEGRAPH_HDRS include/SceneGraph/GLMesh.h)
else()
  message(STATUS "ASSIMP Not found, but required for mesh loading.")
endif()

find_package( DevIL QUIET )
if(IL_FOUND)
  set(HAVE_DEVIL 1)
  list(APPEND REQUIRED_INCLUDE_DIRS "${IL_INCLUDE_DIR};${IL_INCLUDE_DIR}/.." )
  list(APPEND REQUIRED_LIBRARIES "${IL_LIBRARIES};${ILU_LIBRARIES};${ILUT_LIBRARIES}" )
else()
  message(STATUS "DevIL not found; it is required to load other image formats.")
endif()

find_package(PNG QUIET)
if(PNG_FOUND)
  set(HAVE_PNG 1)
  list(APPEND REQUIRED_INCLUDE_DIRS ${PNG_INCLUDE_DIR} )
  list(APPEND REQUIRED_LIBRARIES ${PNG_LIBRARY} )
else()
  message(STATUS "libpng not found - used to load PNG textures")
endif()

find_package(JPEG QUIET)
if(JPEG_FOUND)
  set(HAVE_JPEG 1)
  list(APPEND REQUIRED_INCLUDE_DIRS ${JPEG_INCLUDE_DIR} )
  list(APPEND REQUIRED_LIBRARIES ${JPEG_LIBRARY} )
else()
  message(STATUS "libjpeg not found - used to load Jpeg textures")
endif()

find_package(TIFF QUIET)
if(TIFF_FOUND)
  set(HAVE_TIFF 1)
  list(APPEND REQUIRED_INCLUDE_DIRS ${TIFF_INCLUDE_DIR} )
  list(APPEND REQUIRED_LIBRARIES ${TIFF_LIBRARY} )
else()
  message(STATUS "libtiff not found - used to load TIFF textures")
endif()

# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin CONFIG REQUIRED)
if(Pangolin_FOUND)
  set(HAVE_PANGOLIN 1)
  list(APPEND REQUIRED_INCLUDE_DIRS ${Pangolin_INCLUDE_DIRS})
  list(APPEND REQUIRED_LIBRARIES ${Pangolin_LIBRARIES})
endif()

################################################################################
# Include collected includes / libraries

include_directories( ${REQUIRED_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR}/include )
include_directories( ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR} )
add_library( scenegraph ${SCENEGRAPH_SRCS} )
target_link_libraries( scenegraph ${REQUIRED_LIBRARIES} )

install_library(
  LIBRARY ${LIBRARY_NAME}
  VERSION ${VERSION}
  DESCRIPTION "Allows for the viewing of SceneGraphs"
  HEADERS ${SCENEGRAPH_HDRS}
  HEADER_DESTINATION ${CMAKE_INSTALL_PREFIX}/include/SceneGraph
  INCLUDE_DIRS ${REQUIRED_INCLUDE_DIRS}
  LIB_DEPENDS ${REQUIRED_LIBRARIES}
  LINK_DIRS /usr/local/lib
  )

## Generate symbol export helper header on MSVC
IF(MSVC)
    string(TOUPPER ${LIBRARY_NAME} LIBRARY_NAME_CAPS)
    include(GenerateExportHeader)
    GENERATE_EXPORT_HEADER( ${LIBRARY_NAME}
        BASE_NAME ${LIBRARY_NAME_CAPS}
        EXPORT_MACRO_NAME ${LIBRARY_NAME_CAPS}_EXPORT
        EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/../${LIBRARY_NAME}/${LIBRARY_NAME}_export.h"
        STATIC_DEFINE ${LIBRARY_NAME_CAPS}_BUILT_AS_STATIC
    )
ENDIF()

set( SCENEGRAPH_DIR ${CMAKE_CURRENT_BINARY_DIR} )
set( SCENEGRAPH_LIBRARIES ${REQUIRED_LIBRARIES} scenegraph )
set( SCENEGRAPH_INCLUDE_DIRS ${REQUIRED_INCLUDE_DIRS} )

if(BUILD_EXAMPLES)
    find_package( CVars CONFIG REQUIRED)
    if(CVars_FOUND)
        set(HAVE_CVARS 1)
        list(APPEND REQUIRED_INCLUDE_DIRS ${CVars_INCLUDE_DIRS})
        list(APPEND REQUIRED_LIBRARIES ${CVars_LIBRARIES})
    endif()
  add_subdirectory( Examples )
endif()

include(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
