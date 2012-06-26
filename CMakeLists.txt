project( SceneGraph )
cmake_minimum_required(VERSION 2.8)

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_subdirectory( SceneGraph )
add_subdirectory( Examples )

