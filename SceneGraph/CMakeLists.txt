cmake_minimum_required( VERSION 2.8 )
project( SceneGraph )
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
string( TOLOWER ${PROJECT_NAME} LIBRARY_NAME )

include(def_library)
include(export_package)

################################################################################
# Find required dependencies

find_package( Eigen3 REQUIRED )

# Basic includes / libraries
list( APPEND LIB_INC_DIR "${CMAKE_CURRENT_BINARY_DIR}/..;${CMAKE_CURRENT_SOURCE_DIR}/.." )
list( APPEND USER_INC    ${EIGEN3_INCLUDE_DIR} )

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
    GLLight.cpp
    GLAxis.cpp
    GLCube.cpp
    GLGrid.cpp
    GLDynamicGrid.cpp
)

list(APPEND HEADERS
    config.h.in
    SceneGraph.h
    GLHelpers.h
    GLSLHelpers.h
    GLSceneGraph.h
    GLHeightmap.h
    GLObject.h
    GLGroup.h
    GLCylinder.h
    GLLight.h
    GLShadowLight.h
    GLColor.h
    GLImage.h
    GLVbo.h
    GLText.h
    GLGrid.h
    GLAxis.h
    GLCube.h
    GLTeapot.h
    GLLineStrip.h
    GLPrimitives.h
    GLAxisHistory.h
    GLOpenBox.h
    GLHelpersLoadTextures.h
    GLHelpersBoost.h
    GLHelpersDevil.h
    GLWireSphere.h
    AxisAlignedBoundingBox.h
    LineSegment.h
    PangolinDrawGLObject.h
    PangolinSceneGraphHandler.h
    PangolinImageView.h
)

if( NOT ANDROID )
    list(APPEND SOURCES
        GLHelpers.cpp
        FBO.cpp
        SimCam.cpp
        ../Widgets/nvGlutWidgets.cpp
        ../Widgets/nvGLWidgets.cpp
        ../Widgets/nvWidgets.cpp
    )

    list(APPEND HEADERS
        GLMovableAxis.h
        GLAxisAlignedBox.h
        GLWaypoint.h
        FBO.h
        SimCam.h
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
find_package(Pangolin 0.1 REQUIRED)
if(Pangolin_FOUND)
  set(HAVE_PANGOLIN 1)
  list(APPEND USER_INC ${Pangolin_INCLUDE_DIRS})
  list(APPEND LINK_LIBS ${Pangolin_LIBRARIES})
endif()

################################################################################
# Include collected includes / libraries

include_directories( ${LIB_INC_DIR} )
include_directories( ${USER_INC} )

def_library(${LIBRARY_NAME}
  SOURCES ${SOURCES} ${HEADERS}
  LINK_LIBS ${LINK_LIBS}
  PACKAGE SceneGraph
  )

export_package(SceneGraph
  TARGETS ${LIBRARY_NAME}
  VERSION ${VERSION}
  LIBRARY ${LIBRARY_NAME}
  LIBRARIES ${LIBRARY_NAME}
  INCLUDE_DIRS ${USER_INC} ${LIB_INC_DIR}
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

#######################################################
## Create configure file for inclusion in library

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/config.h"
)

#######################################################
## Install headers / targets

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)
install(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)
