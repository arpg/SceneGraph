project( SimpleGuiTest )
cmake_minimum_required(VERSION 2.8)

find_package( SimpleGui REQUIRED )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( Demo Demo.cpp )
target_link_libraries( Demo ${SimpleGui_LIBRARIES} )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( DemoWithCustomEventHandler DemoWithCustomEventHandler.cpp )
target_link_libraries( DemoWithCustomEventHandler ${SimpleGui_LIBRARIES} )

