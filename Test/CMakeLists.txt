project( SimpleGuiTest )
cmake_minimum_required(VERSION 2.8)

find_package( SimpleGui REQUIRED )

include_directories( ${SimpleGui_INCLUDE_DIRS} )
add_executable( Test Test.cpp InputHandler.cpp )
target_link_libraries( Test ${SimpleGui_LIBRARIES} )

