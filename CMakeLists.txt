project( SceneGraph )
cmake_minimum_required(VERSION 3.0)

# Add to module path, so we can find our cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

################################################################################

option(BUILD_EXAMPLES "Build Examples" ON)

option(BUILD_SHARED_LIBS "Build Shared Library" ON)
set(ANDROID_PACKAGE_NAME "edu.gwu.robotics.scenegraph")

# Overide with cmake -DCMAKE_BUILD_TYPE=Debug {dir}
if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif()

# Platform configuration vars (to generate config.h)
include(SetPlatformVars)

if( NOT MSVC )
  # Set GCC style compiler flags
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -std=c++0x")

  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  endif()
endif()

add_subdirectory( SceneGraph )

if(BUILD_EXAMPLES)
  set(SceneGraph_DIR ${SceneGraph_BINARY_DIR})
  add_subdirectory( Examples )
endif()
