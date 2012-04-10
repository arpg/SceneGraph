set( XML_HDRS rg_tinyxml.h )
set( XML_SRCS rg_tinyxml.cpp rg_tinyxmlparser.cpp rg_tinyxmlerror.cpp )
add_library( xml ${XML_SRCS} ${XML_HDRS}  )

add_to_rg_include_dirs( ${XML_HDRS} )
add_to_rg_libraries( xml )

set( ROBOTGAMES_LIBRARIES ${ROBOTGAMES_LIBRARIES} CACHE INTERNAL "" FORCE )

add_to_rg_link_directories( ${CMAKE_CURRENT_BINARY_DIR} )


