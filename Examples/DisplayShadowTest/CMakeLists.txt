# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.1 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

# Find Scenegraph
find_package(SceneGraph 0.1 REQUIRED)
include_directories(${SceneGraph_INCLUDE_DIRS})

add_executable(DisplayShadowTest main.cpp)
target_link_libraries(DisplayShadowTest ${Pangolin_LIBRARIES})
target_link_libraries(DisplayShadowTest ${SceneGraph_LIBRARIES})
