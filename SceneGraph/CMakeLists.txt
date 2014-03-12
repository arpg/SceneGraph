cmake_minimum_required( VERSION 2.8 )
project( SceneGraph )
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR})
string( TOLOWER ${PROJECT_NAME} LIBRARY_NAME )

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

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
)

list(APPEND HEADERS
    config.h.in
    SceneGraph.h
    GLHelpers.h
    GLSLHelpers.h
    GLSceneGraph.h
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

add_library( ${LIBRARY_NAME} ${SOURCES} ${HEADERS} )
target_link_libraries( ${LIBRARY_NAME} ${LINK_LIBS} )

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

# This relative path allows installed files to be relocatable.
set( CMAKECONFIG_INSTALL_DIR "lib/cmake/${PROJECT_NAME}" )
file( RELATIVE_PATH REL_INCLUDE_DIR
    "${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR}"
    "${CMAKE_INSTALL_PREFIX}/include" )

# Export library for easy inclusion from other cmake projects.
file( REMOVE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake" )
export( TARGETS ${LIBRARY_NAME}
        APPEND FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake" )

# Version information
configure_file("${PROJECT_NAME}ConfigVersion.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Build tree config
set( EXPORT_LIB_INC_DIR "${LIB_INC_DIR}" )
CONFIGURE_FILE( "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY IMMEDIATE )

# Install tree config
set( EXPORT_LIB_INC_DIR "\${${PROJECT_NAME}_CMAKE_DIR}/${REL_INCLUDE_DIR}" )
configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY )

# Add package to CMake package registery for use from the build tree
option( EXPORT_${PROJECT_NAME}
  "Should the ${PROJECT_NAME} package be exported for use by other software" ON )

if( EXPORT_${PROJECT_NAME} )
  export( PACKAGE ${PROJECT_NAME} )
endif()

#######################################################
## Install headers / targets

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/config.h"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)
install(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
)
install(TARGETS ${LIBRARY_NAME}
  EXPORT "${PROJECT_NAME}Targets"
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

#######################################################
## Install CMake config

INSTALL(
    FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKECONFIG_INSTALL_DIR} )

install( EXPORT "${PROJECT_NAME}Targets" DESTINATION ${CMAKECONFIG_INSTALL_DIR} )
