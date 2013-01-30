cmake_minimum_required( VERSION 2.8 )

project( SimCamDEMO )

find_package( Pangolin REQUIRED )
find_package( SceneGraph REQUIRED )
find_package( MVL REQUIRED )
find_package( CVARS REQUIRED )
find_package( OpenCV REQUIRED )

include_directories( ${Pangolin_INCLUDE_DIRS} )
include_directories( ${SceneGraph_INCLUDE_DIRS} )
include_directories( ${MVL_INCLUDE_DIRS} )
include_directories( ${CVARS_INCLUDE_DIR} )
include_directories( ${OpenCV2_INCLUDE_DIRS} )

link_directories( ${Pangolin_LIBRARY_DIRS} )

list(APPEND HDRS
    CVarHelpers.h
)

list(APPEND SRCS
    SimCamDEMO.cpp
)

add_executable( SimCamDEMO ${HDRS} ${SRCS} )

target_link_libraries( SimCamDEMO
                        ${Pangolin_LIBRARIES}
                        ${SceneGraph_LIBRARIES}
                        ${MVL_LIBRARIES}
                        ${CVARS_LIBRARIES}
                        ${OpenCV2_LIBRARIES}
                     )
