# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.1 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

# Find Scenegraph
find_package(SceneGraph 0.1 REQUIRED)
include_directories(${SceneGraph_INCLUDE_DIRS})

find_package(Bullet QUIET)
if(BULLET_FOUND)
    include_directories(${BULLET_INCLUDE_DIRS})

    # We require Bullet to be built with double precision
    add_definitions(-DBT_USE_DOUBLE_PRECISION)
    add_executable(ModelPhysics main.cpp)
    target_link_libraries(ModelPhysics ${Pangolin_LIBRARIES})
    target_link_libraries(ModelPhysics ${SceneGraph_LIBRARIES})
    target_link_libraries(ModelPhysics ${BULLET_LIBRARIES})

    add_executable( CarDemo CarDemo.cpp )
    target_link_libraries(CarDemo ${Pangolin_LIBRARIES})
    target_link_libraries(CarDemo ${SceneGraph_LIBRARIES})
    target_link_libraries(CarDemo ${BULLET_LIBRARIES})
endif()
