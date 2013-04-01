project( SceneGraph )
cmake_minimum_required(VERSION 2.8)

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
