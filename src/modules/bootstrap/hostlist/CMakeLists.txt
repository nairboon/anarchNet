project(anarchNet)

################################################################################
# 			Hostlist Bootstrap	                               #
################################################################################

set(HOSTLIST_SRC hostlist.cc
	      hostlist.h)
add_library(bs_hostlist SHARED ${HOSTLIST_SRC})
target_link_libraries(bs_hostlist an)
