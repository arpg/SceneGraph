# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

# Find SceneGraph
include_directories(${CMAKE_BINARY_DIR}/Examples/Display ${SceneGraph_INCLUDE_DIRS})

# Set GCC style compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(Display main.cpp)
target_link_libraries(Display ${Pangolin_LIBRARIES})
target_link_libraries(Display ${SceneGraph_LIBRARIES})
