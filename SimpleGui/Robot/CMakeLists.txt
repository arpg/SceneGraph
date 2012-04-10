set( ROBOT_HDRS urdf_parser.h color.h joint.h link.h model.h pose.h )
set( ROBOT_SRCS urdf_parser.cpp joint.cpp link.cpp )
#add_library( robot ${ROBOT_SRCS} ${ROBOT_HDRS} )
#target_link_libraries( robot xml )

#add_executable( urdf_parser_test urdf_parser_test.cpp )
#target_link_libraries( urdf_parser_test robot )

#add_executable( urdf_parser_test urdf_parser_test.cpp ${ROBOT_SRCS} ${ROBOT_HDRS} )
#target_link_libraries( urdf_parser_test robot )
