project(anarchNet)

################################################################################
# 			File Logger		                               #
################################################################################

set(LOGGER_SRC 	file_logger.cc
	      	file_logger.h
	)
add_library(util_file_logger SHARED ${LOGGER_SRC})
target_link_libraries(util_file_logger an)
