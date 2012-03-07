project( MochaGui )
cmake_minimum_required(VERSION 2.8)

# Add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )

#the boost libraries that we need
set(BOOST_LIBS thread)


find_package( CarPlanner REQUIRED )
find_package( Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_package( FLTK REQUIRED )
find_package( CVARS REQUIRED )
find_package( OpenGL REQUIRED )
find_package( OpenCV REQUIRED )
find_package( Eigen3 REQUIRED )
find_package( ZeroMQ REQUIRED )
find_package( Protobuf REQUIRED )
find_package( ASSIMP REQUIRED )

include_directories( . )
include_directories( ${CAR_PLANNER_INCLUDE_DIRS} )
include_directories( ${BOOST_INCLUDE_DIR} )
include_directories( ${FLTK_INCLUDE_DIR} )
include_directories( ${CVARS_INCLUDE_DIR} )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories( ${EIGEN3_INCLUDE_DIR} )
include_directories( ${ZeroMQ_INCLUDE_DIRS} )
include_directories( ${PROTOBUF_INCLUDE_DIRS} )
include_directories( ${ASSIMP_INCLUDE_DIR} )

include_directories( ${CMAKE_BINARY_DIR} )
PROTOBUF_GENERATE_CPP( PROTO_SRCS PROTO_HDRS Messages.proto )

set( hdrs   GLWindow.h
            GLHelpers.h
            GLColor.h
            GLMesh.h
            GLText.h
            GLGrid.h
            GLLineStrip.h
            GLCar.h
            GLRamp.h
            GLWayPoint.h
            AssetLoader.h
            Messages.pb.h
   )

set( srcs   GLWindow.cpp Messages.pb.cc )

add_executable( MochaGui ${srcs} ${hdrs} )
target_link_libraries( MochaGui ${CAR_PLANNER_LIBRARIES} )
target_link_libraries( MochaGui ${Boost_THREAD_LIBRARY} )
target_link_libraries( MochaGui ${CVARS_LIBRARIES} )
target_link_libraries( MochaGui ${FLTK_LIBRARIES} )
target_link_libraries( MochaGui ${OPENGL_LIBRARIES} )
target_link_libraries( MochaGui ${OpenCV_LIBS} )
target_link_libraries( MochaGui ${ZeroMQ_LIBRARIES} ${PROTOBUF_LIBRARIES} )
target_link_libraries( MochaGui ${ASSIMP_LIBRARIES} )

