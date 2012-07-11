project(anarchNet)

################################################################################
# 			Block Store	                               #
################################################################################

set(BS_SRC 	blockstore.cc
	      	blockstore.h
	)
add_library(ls_blockstore SHARED ${BS_SRC})
target_link_libraries(ls_blockstore an)
