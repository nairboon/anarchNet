#
# anarchNet 
#

project(anarchNet)
cmake_minimum_required(VERSION 2.6)

GET_FILENAME_COMPONENT(CMAKE_BUILD_TYPE_DIR ${CMAKE_BINARY_DIR} NAME)
SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_DIR})


set(ROOT_DIR  ${CMAKE_SOURCE_DIR})
set(SOURCE_DIR  "${CMAKE_SOURCE_DIR}/src")

if(APPLE AND CMAKE_GENERATOR MATCHES "Xcode")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/..")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/..")
	set(BIN_DIR "${CMAKE_BINARY_DIR}")
	set(PLG_BIN_DIR ${BIN_DIR})
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_BINARY_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
	set(BIN_DIR "${CMAKE_BINARY_DIR}/bin")
	set(PLG_BIN_DIR ${BIN_DIR})
endif()

###################################################################################################
# Dependencies
###################################################################################################

find_package(GTest REQUIRED)
find_library(GCRYPT NAMES gcrypt REQUIRED)
#find_library(CRYPTOPP NAMES cryptopp_d REQUIRED PATH)
find_library(LSQL NAMES litesql REQUIRED PATH)
find_library(LSQLUTIL NAMES litesql-util REQUIRED PATH)
set(LITESQL ${LSQL} ${LSQLUTIL})
#find_library(UDT NAMES udt_d REQUIRED)
#find_library(SQLITE NAMES libsqlit3 sqlite3 REQUIRED)
#find_library(ARCHIVE NAMES libarchive archive REQUIRED)
find_library(BOOST_LOG NAMES libboost_log.a boost_log PATHS "${CMAKE_SOURCE_DIR}/third-party/boost-log/")

set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0")
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost 1.47 COMPONENTS system program_options thread filesystem date_time serialization REQUIRED)

include_directories( 
	"${SOURCE_DIR}/daemon"
	"${SOURCE_DIR}/library"
	${Boost_INCLUDE_DIRS} 
	${GTEST_INCLUDE_DIRS}
	"${SOURCE_DIR}/rpc"
	"${CMAKE_SOURCE_DIR}/third-party/"
	"${CMAKE_SOURCE_DIR}/third-party/pugg/include"
	"${CMAKE_SOURCE_DIR}/third-party/jsoncpp/include"
	"${CMAKE_SOURCE_DIR}/third-party/jsonrpc/src"
	"${CMAKE_SOURCE_DIR}/third-party/boost-log"
	"${CMAKE_SOURCE_DIR}/third-party/diff_match_patch-stl"
	"/usr/local/include"
	"/sw/include"
 )
set( LIB
	${Boost_LIBRARIES}
	#${CRYPTOPP}
	#${SQLITE}
	${ARCHIVE}
	${BOOST_LOG}
	${GCRYPT}
	)
set( TEST_LIB
	${GCOV}
	${GTEST_LIBRARIES}
	)
if(UNIX)
set(PLATFORM_LIB pthread dl)
endif()

message(STATUS "linked libraries: ${LIB} ${TEST_LIB}")

################################################################################
# json
################################################################################

file(GLOB JSON_C "${ROOT_DIR}/third-party/boost/json/src/*.c*")
file(GLOB JSON_H "${ROOT_DIR}/third-party/boost/json/*.h*")
list(APPEND JSON_SRC ${JSON_C} ${JSON_H})
add_library(json ${JSON_SRC})

################################################################################
# jsonrpc
################################################################################

file(GLOB JSON_RPC_C "${ROOT_DIR}/third-party/jsonrpc/src/*.c*")
file(GLOB JSON_RPC_H "${ROOT_DIR}/third-party/jsonrpc/src/*.h*")
list(APPEND JSON_RPC_SRC ${JSON_RPC_C} ${JSON_RPC_H})

#add_library(jsonrpc ${JSON_RPC_SRC})

#add_dependencies(jsonrpc json)

################################################################################
# rpc
################################################################################

file(GLOB RPC_C "${SOURCE_DIR}/rpc/*.c*")
file(GLOB RPC_H "${SOURCE_DIR}/rpc/*.h*")
list(APPEND RPC_SRC ${RPC_C} ${RPC_H} ${JSON_RPC_H} "${ROOT_DIR}/third-party/jsonrpc/src/jsonrpc_handler.cpp")

add_library(rpc ${RPC_SRC})

add_dependencies(rpc json logger)
################################################################################
# logger
################################################################################

set(LOGGER_SRC "${SOURCE_DIR}/daemon/logger.h" "${SOURCE_DIR}/daemon/logger.cc")
add_library(logger ${LOGGER_SRC})

################################################################################
# Daemon
################################################################################
set(DAEMON_SRC  "${SOURCE_DIR}/daemon/anarchNet.h"
		"${SOURCE_DIR}/daemon/config.h"
		"${SOURCE_DIR}/daemon/singleton.h"
		"${SOURCE_DIR}/daemon/version.h"
		"${SOURCE_DIR}/daemon/config_manager.h"
		"${SOURCE_DIR}/daemon/db_manager.h"
		"${SOURCE_DIR}/daemon/module_manager.h"
		"${SOURCE_DIR}/daemon/net_manager.h"
		"${SOURCE_DIR}/daemon/plugin_manager.h"
		"${SOURCE_DIR}/daemon/rpc_manager.h"
		"${SOURCE_DIR}/daemon/db.h"
		"${SOURCE_DIR}/daemon/logger.h"
		"${SOURCE_DIR}/daemon/daemon.h"
		"${SOURCE_DIR}/daemon/crypto.cc"
		"${SOURCE_DIR}/daemon/crypto.h"
		"${SOURCE_DIR}/daemon/config_manager.cc"
		"${SOURCE_DIR}/daemon/db_manager.cc"
		"${SOURCE_DIR}/daemon/module_manager.cc"
		"${SOURCE_DIR}/daemon/net_manager.cc"
		"${SOURCE_DIR}/daemon/plugin_manager.cc"
		"${SOURCE_DIR}/daemon/rpc_manager.cc"
		"${SOURCE_DIR}/daemon/db.cc"
		"${SOURCE_DIR}/daemon/logger.cc"
		"${SOURCE_DIR}/daemon/daemon.cc"
		"${SOURCE_DIR}/daemon/plugins/bootstrap.h"
		"${SOURCE_DIR}/daemon/plugins/localstorage.h"
		"${SOURCE_DIR}/daemon/plugins/remotestorage.h"
		"${SOURCE_DIR}/daemon/plugins/plugin.h"
		"${SOURCE_DIR}/daemon/plugins/session.h"
		"${ROOT_DIR}/third-party/diff_match_patch-stl/diff_match_patch.h"
		"${SOURCE_DIR}/daemon/main.cc"
)


add_executable(anDaemon ${DAEMON_SRC})
add_dependencies(anDaemon json logger rpc)
target_link_libraries(anDaemon ${LIB} ${PLATFORM_LIB} json logger rpc)

configure_file(${ROOT_DIR}/scripts/startDaemon.sh ${BIN_DIR}/startDaemon.sh)
configure_file(${ROOT_DIR}/scripts/stopDaemon.sh ${BIN_DIR}/stopDaemon.sh)
add_custom_command(TARGET anDaemon POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E create_symlink "${ROOT_DIR}/contrib/anarchnet.conf" "${BIN_DIR}/anarchnet.conf")
################################################################################
# Library
################################################################################

file(GLOB LIBRARY_C "${SOURCE_DIR}/library/*.c*")
file(GLOB LIBRARY_H "${SOURCE_DIR}/library/*.h*")
list(APPEND LIBRARY_SRC ${LIBRARY_C} ${LIBRARY_H})

add_library(an SHARED ${LIBRARY_SRC})
add_dependencies(an rpc logger)
target_link_libraries(an ${LIB} json logger rpc)

################################################################################
# Python library search (style check)                                          #
################################################################################
unset(PYTHON_EXECUTABLE CACHE)
include(FindPythonInterp)
include(FindPythonLibs)
find_package(PythonInterp)
if (PYTHONINTERP_FOUND)
  message("-- Found python executable: style checking enabled.")
else ()
  message("-- Didn't find python executable: style checking disabled.")
endif ()

###################################################################################################
# Tests
###################################################################################################
file(GLOB T_MAIN  "${SOURCE_DIR}/tests/main.cc")
file(GLOB T_MAIND  "${SOURCE_DIR}/tests/main_daemon.cc")

file(GLOB T_BASE_C "${SOURCE_DIR}/tests/base/*.c*")
file(GLOB T_BASE_H "${SOURCE_DIR}/tests/base/*.h*")
list(APPEND T_BASE_DIR ${T_BASE_C} ${T_BASE_H})

file(GLOB T_RPC_C "${SOURCE_DIR}/tests/rpc/*.c*")
file(GLOB T_RPC_H "${SOURCE_DIR}/tests/rpc/*.h*")
list(APPEND T_RPC_DIR ${T_RPC_C} ${T_RPC_H})

file(GLOB T_MD_C "${SOURCE_DIR}/tests/modules/*.c*")
file(GLOB T_MD_H "${SOURCE_DIR}/tests/modules/*.h*")
set(T_MD_D 	"${SOURCE_DIR}/daemon/module_manager.cc"
		"${SOURCE_DIR}/daemon/db_manager.cc"
		"${SOURCE_DIR}/daemon/daemon.cc"
		"${SOURCE_DIR}/daemon/plugin_manager.cc"
		"${SOURCE_DIR}/daemon/net_manager.cc"
		"${SOURCE_DIR}/daemon/rpc_manager.cc"
		"${SOURCE_DIR}/daemon/config_manager.cc"
		"${SOURCE_DIR}/daemon/db.cc"
		"${SOURCE_DIR}/daemon/crypto.cc"
)
list(APPEND T_MD_DIR ${T_MD_C} ${T_MD_H} ${T_MD_D})

add_executable(TBase ${T_MAIN} ${T_BASE_DIR})
add_executable(TRPC ${T_MAIN} ${T_RPC_DIR})
add_executable(TModule ${T_MAIND} ${T_MD_DIR})

add_dependencies(TBase anDaemon logger)
add_dependencies(TRPC anDaemon logger rpc)
add_dependencies(TModule anDaemon logger rpc)

target_link_libraries(TBase ${LIB} ${TEST_LIB} json an logger)
target_link_libraries(TRPC ${LIB} ${TEST_LIB} json an logger rpc)
target_link_libraries(TModule ${LIB} ${TEST_LIB} json an logger rpc)

enable_testing()
add_test(StartDaemon sh ${BIN_DIR}/startDaemon.sh)
add_test(Base ${BIN_DIR}/TBase)
add_test(RPC ${BIN_DIR}/TRPC)
add_test(StopDaemon sh ${BIN_DIR}/stopDaemon.sh)

add_test(Modules ${BIN_DIR}/TModule)

if (PYTHONINTERP_FOUND)
  set(CPPLINTOPS "--verbose=4" "--filter=-whitespace/tab,-whitespace/end_of_line,-build/include,-readability/streams")
  if (UNIX)
    add_test(STYLE_CHECK_DAEMON ${ROOT_DIR}/cpplint.py ${CPPLINTOPS} ${DAEMON_SRC})
    set_property(TEST STYLE_CHECK_DAEMON PROPERTY LABELS Functional Daemon CodingStyle)
   # ADD_TEST(STYLE_CHECK_LIBRARY ${MAIDSAFE_SOURCE_DIR}/cpplint.py ${M_BASE_DIR} ${T_BASE_DIR})
   # SET_PROPERTY(TEST STYLE_CHECK_LIBRARY PROPERTY LABELS Functional Library CodingStyle)
  else ()
    add_test(STYLE_CHECK finderrorsall.bat)
    set_property(TEST STYLE_CHECK PROPERTY LABELS Functional CodingStyle)
  endif ()
endif ()


 MACRO (MACRO_OPTIONAL_ADD_SUBDIRECTORY _dir )
    GET_FILENAME_COMPONENT(_fullPath ${_dir} ABSOLUTE)
   IF(EXISTS "${_fullPath}/CMakeLists.txt")
       IF(DISABLE_ALL_OPTIONAL_SUBDIRECTORIES)
          SET(_DEFAULT_OPTION_VALUE FALSE)
         ELSE()
                    SET(_DEFAULT_OPTION_VALUE TRUE)
       ENDIF()
	OPTION(BUILD_${_dir} "Build directory ${_dir}" TRUE)
       IF(BUILD_${_dir})
          ADD_SUBDIRECTORY(${_dir})
       ENDIF()
   ENDIF()
 ENDMACRO (MACRO_OPTIONAL_ADD_SUBDIRECTORY)

MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/bootstrap")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/localstorage")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/remotestorage")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/session")
