project( SceneGraph )
cmake_minimum_required(VERSION 2.8)


################################################################################
# Platform tests for config.h

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

IF(ANDROID)
   SET(_ANDROID_ 1)
ENDIF()


# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")

option(BUILD_EXAMPLES "Build Examples" ON)

add_subdirectory( SceneGraph )

if(BUILD_EXAMPLES)
    set(SCENEGRAPH_EXAMPLE 1)
    add_subdirectory( Examples )
endif()
