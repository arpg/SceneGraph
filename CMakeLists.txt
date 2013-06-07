project( SceneGraph )
cmake_minimum_required(VERSION 2.8)

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

################################################################################

option(BUILD_EXAMPLES "Build Examples" ON)

if(NOT ANDROID)
   option(BUILD_SHARED_LIBS "Build Shared Library" ON)
else()
   set(BUILD_SHARED_LIBS OFF)
endif()

# Overide with cmake -DCMAKE_BUILD_TYPE=Debug {dir}
if( NOT CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE Release )
endif()

# Platform configuration vars (to generate config.h)
include(SetPlatformVars)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")

add_subdirectory( SceneGraph )

if(BUILD_EXAMPLES)
    add_subdirectory( Examples )
endif()
