# Find Pangolin (https://github.com/stevenlovegrove/Pangolin)
find_package(Pangolin 0.1 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

# Find Scenegraph
include_directories(${CMAKE_BINARY_DIR}/Examples/ModelPhysics ${SCENEGRAPH_INCLUDE_DIRS})

# Set GCC style compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Bullet QUIET)
if(BULLET_FOUND)
    include_directories(${BULLET_INCLUDE_DIRS})

    # We require Bullet to be built with double precision
    add_definitions(-DBT_USE_DOUBLE_PRECISION)
    add_executable(ModelPhysics main.cpp)
    target_link_libraries(ModelPhysics ${Pangolin_LIBRARIES})
    target_link_libraries(ModelPhysics ${SCENEGRAPH_LIBRARIES})
    target_link_libraries(ModelPhysics ${BULLET_LIBRARIES})

    add_executable( CarDemo CarDemo.cpp )
    target_link_libraries(CarDemo ${Pangolin_LIBRARIES})
    target_link_libraries(CarDemo ${SCENEGRAPH_LIBRARIES})
    target_link_libraries(CarDemo ${BULLET_LIBRARIES})
    target_link_libraries(CarDemo ${CVars_LIBRARIES})
endif()
