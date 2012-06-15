project(anarchNet)

################################################################################
# 			Flat File Storage	                               #
################################################################################

set(FF_SRC 	ffstore.cc
	      	ffstore.h
		)
add_library(ls_flatfile SHARED ${FF_SRC})
target_link_libraries(ls_flatfile an)

