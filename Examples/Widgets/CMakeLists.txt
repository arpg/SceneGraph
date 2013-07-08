# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.1 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

# Find Scenegraph
find_package(SceneGraph 0.1 REQUIRED)
include_directories(${SceneGraph_INCLUDE_DIRS})

add_executable(Widgets main.cpp)
target_link_libraries(Widgets ${Pangolin_LIBRARIES})
target_link_libraries(Widgets ${SceneGraph_LIBRARIES})
