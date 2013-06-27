# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
FIND_PACKAGE(Pangolin 0.1 REQUIRED)
INCLUDE_DIRECTORIES(${Pangolin_INCLUDE_DIRS})
LINK_DIRECTORIES(${Pangolin_LIBRARY_DIRS})
LINK_LIBRARIES(${Pangolin_LIBRARIES})

# Find Scenegraph
FIND_PACKAGE(SceneGraph 0.1 REQUIRED)
INCLUDE_DIRECTORIES(${SceneGraph_INCLUDE_DIRS})
LINK_LIBRARIES(${SceneGraph_LIBRARIES})

ADD_EXECUTABLE(DisplayMultiView main.cpp)
