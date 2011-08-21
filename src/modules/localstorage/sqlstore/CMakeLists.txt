project(anarchNet)

########################################################################
# 		Sqlstore			        	       #
########################################################################

set(SQLSTORE_SRC sqlstore.cc
	      	sqlstore.h
		sqldb.cc
	     	sqldb.h)

add_library(ls_sqlstore SHARED ${SQLSTORE_SRC})
target_link_libraries(ls_sqlstore an ${LITESQL})
