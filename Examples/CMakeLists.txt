project( SimpleGuiTest )
cmake_minimum_required(VERSION 2.8)

find_package( SimpleGui REQUIRED )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( Demo Demo.cpp )
target_link_libraries( Demo ${SimpleGui_LIBRARIES} )
add_dependencies( Demo SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( DemoWithCustomEventHandler DemoWithCustomEventHandler.cpp )
target_link_libraries( DemoWithCustomEventHandler ${SimpleGui_LIBRARIES} )
add_dependencies( DemoWithCustomEventHandler SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( ShaderDemo ShaderDemo.cpp )
target_link_libraries( ShaderDemo ${SimpleGui_LIBRARIES} )
add_dependencies( ShaderDemo SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( SimCamDemo SimCamDemo.cpp )
target_link_libraries( SimCamDemo ${SimpleGui_LIBRARIES} )
add_dependencies( SimCamDemo SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( PBOTest PBOTest.cpp )
target_link_libraries( PBOTest ${SimpleGui_LIBRARIES} )
add_dependencies( PBOTest SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( Axis_Demo RenderAxis.cpp )
target_link_libraries( Axis_Demo ${SimpleGui_LIBRARIES} )
add_dependencies( Axis_Demo SimpleGui )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( HELLo HELLo.cpp )
target_link_libraries( HELLo ${SimpleGui_LIBRARIES} )
add_dependencies( HELLo SimpleGui )


