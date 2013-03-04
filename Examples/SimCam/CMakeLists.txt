cmake_minimum_required( VERSION 2.8 )

project( SimCamDEMO )

find_package( Pangolin REQUIRED )
find_package( SceneGraph REQUIRED )
find_package( MVL REQUIRED )

include_directories( ${Pangolin_INCLUDE_DIRS} )
include_directories( ${SceneGraph_INCLUDE_DIRS} )
include_directories( ${MVL_INCLUDE_DIRS} )

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
                     )

# In your own code, you can ommit these lines
IF( ${SCENEGRAPH_EXAMPLE} )
  add_dependencies(SimCamDEMO SceneGraph)
ENDIF()
