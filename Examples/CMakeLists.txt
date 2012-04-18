project( SimpleGuiTest )
cmake_minimum_required(VERSION 2.8)

find_package( SimpleGui REQUIRED )

include_directories( ${SimpleGui_INCLUDE_DIRS} )


set( BuildDemo false CACHE "" force )
if( BuildDemo )
    add_executable( Demo Demo.cpp )
    target_link_libraries( Demo ${SimpleGui_LIBRARIES} )
    add_dependencies( Demo SimpleGui )
endif()

set( BuildDemoWithCustomHandler false CACHE "" force )
if( BuildDemoWithCustomHandler )
    add_executable( DemoWithCustomEventHandler DemoWithCustomEventHandler.cpp )
    target_link_libraries( DemoWithCustomEventHandler ${SimpleGui_LIBRARIES} )
    add_dependencies( DemoWithCustomEventHandler SimpleGui )
endif()

set( BuildShaderDemo false CACHE "" force )
if( BuildShaderDemo )
    add_executable( ShaderDemo ShaderDemo.cpp )
    target_link_libraries( ShaderDemo ${SimpleGui_LIBRARIES} )
    add_dependencies( ShaderDemo SimpleGui )
endif()

set( BuildSimCamDemo false CACHE "" force )
if( BuildSimCamDemo )
    add_executable( SimCamDemo SimCamDemo.cpp )
    target_link_libraries( SimCamDemo ${SimpleGui_LIBRARIES} )
    add_dependencies( SimCamDemo SimpleGui )
endif()

set( BuildRangeCamDemo false CACHE "" force )
if( BuildRangeCamDemo )
    add_executable( RangeCamDemo RangeCamDemo.cpp )
    target_link_libraries( RangeCamDemo ${SimpleGui_LIBRARIES} )
    add_dependencies( RangeCamDemo SimpleGui )
endif()

set( BuildPBOTest false CACHE "" force )
if( BuildPBOTest )
    add_executable( PBOTest PBOTest.cpp )
    target_link_libraries( PBOTest ${SimpleGui_LIBRARIES} )
    add_dependencies( PBOTest SimpleGui )
endif()



set( BuildAxis_Demo true CACHE "" force )
if( BuildAxis_Demo )
    add_executable( Axis_Demo RenderAxis.cpp )
    target_link_libraries( Axis_Demo ${SimpleGui_LIBRARIES} )
    add_dependencies( Axis_Demo SimpleGui )
endif()

set( BuildHello false CACHE "" force )
if( BuildHello )
    add_executable( Hello Hello.cpp )
    target_link_libraries( Hello ${SimpleGui_LIBRARIES} )
    add_dependencies( Hello SimpleGui )
endif()

