# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
FIND_PACKAGE(Pangolin REQUIRED)
INCLUDE_DIRECTORIES(${Pangolin_INCLUDE_DIRS})
LINK_DIRECTORIES(${Pangolin_LIBRARY_DIRS})
LINK_LIBRARIES(${Pangolin_LIBRARIES})

# Find Scenegraph
FIND_PACKAGE(SceneGraph REQUIRED)
INCLUDE_DIRECTORIES(${SceneGraph_INCLUDE_DIRS})
LINK_LIBRARIES(${SceneGraph_LIBRARIES})

FIND_PACKAGE(Bullet QUIET)
IF(BULLET_FOUND)
    INCLUDE_DIRECTORIES(${BULLET_INCLUDE_DIRS})
    LINK_LIBRARIES(${BULLET_LIBRARIES})
    ADD_EXECUTABLE(ModelPhysics main.cpp)
    ADD_EXECUTABLE( CarDemo CarDemo.cpp )

    # In your own code, you can ommit these lines
    IF( ${SCENEGRAPH_EXAMPLE} )
      add_dependencies(ModelPhysics SceneGraph)
      add_dependencies(CarDemo SceneGraph)
    ENDIF()
ENDIF()
