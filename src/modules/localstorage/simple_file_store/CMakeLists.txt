project(anarchNet)

################################################################################
# 			Simple File Storage	                               #
################################################################################

set(SFS_SRC 	sfs.cc
	      	sfs.h
	)
add_library(ls_simple_file_store SHARED ${SFS_SRC})
target_link_libraries(ls_simple_file_store an)
