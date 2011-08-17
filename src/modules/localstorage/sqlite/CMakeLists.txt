project(anarchNet)

################################################################################
# 			Sqlite Storage	                               #
################################################################################

set(SQLITE_SRC 	sqlite_store.cc
	      	sqlite_store.h
		cpp_sqlite3.cc
	     	cpp_sqlite3.h)
add_library(bs_sqlite SHARED ${SQLITE_SRC})
target_link_libraries(bs_sqlite an)

message(${BIN_DIR})