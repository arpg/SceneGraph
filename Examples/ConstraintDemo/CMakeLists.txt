# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
FIND_PACKAGE(Pangolin REQUIRED)
INCLUDE_DIRECTORIES(${Pangolin_INCLUDE_DIRS})
LINK_DIRECTORIES(${Pangolin_LIBRARY_DIRS})
LINK_LIBRARIES(${Pangolin_LIBRARIES})

# Find Scenegraph
FIND_PACKAGE(SceneGraph REQUIRED)
INCLUDE_DIRECTORIES(${SceneGraph_INCLUDE_DIRS})
LINK_LIBRARIES(${SceneGraph_LIBRARIES})

find_package(ModelGraph REQUIRED)
include_directories(${RPG_INCLUDE_DIRS})

find_package( Bullet REQUIRED )
include_directories(${BULLET_INCLUDE_DIRS})
link_libraries(${BULLET_LIBRARIES})


ADD_EXECUTABLE(ContsraintDemo main.cpp)

# In your own code, you can ommit these lines
IF( ${SCENEGRAPH_EXAMPLE} )
  add_dependencies(ContsraintDemo SceneGraph)
ENDIF()
