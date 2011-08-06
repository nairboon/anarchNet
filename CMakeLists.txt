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

find_library(GLOG NAMES libglog.a glog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)
#find_library(MAIDSAFE NAMES maidsafe_dht_d libmaidsafe_dht_d.a maidsafe REQUIRED)
find_library(CRYPTOPP NAMES cryptopp_d REQUIRED PATH)
find_library(UDT NAMES udt_d REQUIRED)
find_library(SQLITE NAMES libsqlit3 sqlite3 REQUIRED)
find_library(ARCHIVE NAMES libarchive archive REQUIRED)


set(Boost_ADDITIONAL_VERSIONS "1.47" "1.47.0")
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost 1.47 COMPONENTS system program_options thread filesystem REQUIRED)

include_directories( 
	"${SOURCE_DIR}"
	"${SOURCE_DIR}/daemon"
	"${SOURCE_DIR}/library"
	"/usr/local/include"
	${Boost_INCLUDE_DIRS} 
	${GTEST_INCLUDE_DIRS}
	#${MAIDSAFE_INCLUDE_DIRS}
	"${SOURCE_DIR}/daemon/protocol"
	"${ROOT_DIR}/third-party/protobuf-jerpc/rpc"
	"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin"
	"${CMAKE_SOURCE_DIR}/third-party/pugg/include"
	"${CMAKE_SOURCE_DIR}/third-party/CppSQLite3"
 )
set( LIB
	${Boost_LIBRARIES}
	${GLOG}
	${GCOV}
	${GTEST_LIBRARIES}
	${PROTOBUF_LIBRARIES}
	${UDT}
	${CRYPTOPP}
	${SQLITE}
	${ARCHIVE}
	)

if(UNIX)
set(PLATFORM_LIB pthread dl)
endif()

message(STATUS "linked libraries: ${LIB}")

###################################################################################################
# Protobuf
###################################################################################################

#Function to generate CC and header files derived from proto files
macro(generate_protobuf_files)

 foreach(FIL ${ARGV})
    get_filename_component(ABS_FIL ${FIL} NAME)
    get_filename_component(FIL_WE ${FIL} NAME_WE)
    get_filename_component(FILES_PATH ${FIL} PATH)

   list(APPEND SRCS "${FILES_PATH}/${FIL_WE}.pb.cc")
    list(APPEND HDRS "${FILES_PATH}/${FIL_WE}.pb.h")

    message("Marked for compiling (protoc): ${FIL}")
    get_target_property(PLUGIN_EXE protoc-gen-rpc LOCATION)

	execute_process(COMMAND cmake -E remove "${FILES_PATH}/${FIL_WE}.pb.h")
	execute_process(COMMAND cmake -E remove "${FILES_PATH}/${FIL_WE}.pb.cc")

	add_custom_command( OUTPUT "${FILES_PATH}/${FIL_WE}.pb.cc"
             			   "${FILES_PATH}/${FIL_WE}.pb.h"
			    WORKING_DIRECTORY ${FILES_PATH}
			    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} "-I${FILES_PATH}" "--cpp_out=${FILES_PATH}" "--plugin=protoc-gen-rpc=${PLUGIN_EXE}" "--rpc_out=${FILES_PATH}" ${FIL}
			    DEPENDS protoc-gen-rpc)
  endforeach()
set_source_files_properties(${SRCS} ${HDRS} PROPERTIES GENERATED 1)
endmacro()

### generate rpc plugin ###
set(RPC_PLG_SRC "${ROOT_DIR}/third-party/protobuf-jerpc/plugin/file_generator.cc"
		"${ROOT_DIR}/third-party/protobuf-jerpc/plugin/helpers.cc"
		"${ROOT_DIR}/third-party/protobuf-jerpc/plugin/plugin.cc"
		"${ROOT_DIR}/third-party/protobuf-jerpc/plugin/service_generator.cc"
		"${ROOT_DIR}/third-party/protobuf-jerpc/plugin/service.cc")
add_executable(protoc-gen-rpc ${RPC_PLG_SRC})
target_link_libraries(protoc-gen-rpc ${PROTOBUF_LIBRARIES} ${PROTOBUF_PROTOC_LIBRARY} ${PLATFORM_LIB})

### generate proto files ###

set(PROTO_FILES "${SOURCE_DIR}/daemon/protocol/util.proto"
       			"${SOURCE_DIR}/daemon/protocol/protorpc.proto"
				"${SOURCE_DIR}/daemon/protocol/rpc_service.proto"
				"${SOURCE_DIR}/daemon/protocol/p2p_service.proto")
#if(${REGENERATE})
	generate_protobuf_files(${PROTO_FILES})
	MESSAGE("generated protobuf files")
#endif()

set(PB_DAEMON_G ${SRCS} ${HDRS})

file(GLOB PB-RPC_C "${ROOT_DIR}/third-party/protobuf-jerpc/rpc/*.c*")
file(GLOB PB-RPC_H "${ROOT_DIR}/third-party/protobuf-jerpc/rpc/*.h*")

list(APPEND PB-RPC_SRC ${PB-RPC_C} ${PB-RPC_H}
		   "${SOURCE_DIR}/daemon/protocol/protorpc.pb.cc"
		   "${SOURCE_DIR}/daemon/protocol/protorpc.pb.h"
		   "${SOURCE_DIR}/daemon/protocol/util.pb.cc"
		   "${SOURCE_DIR}/daemon/protocol/util.pb.h"
		   "${SOURCE_DIR}/daemon/protocol/rpc_service.pb.cc"
		   "${SOURCE_DIR}/daemon/protocol/rpc_service.pb.h"
		   "${SOURCE_DIR}/daemon/protocol/p2p_service.pb.cc"
		   "${SOURCE_DIR}/daemon/protocol/p2p_service.pb.h"
		   "${ROOT_DIR}/third-party/protobuf-jerpc/plugin/service.cc"
)

add_library(pb-rpc ${PB-RPC_SRC})
add_dependencies(pb-rpc protoc-gen-rpc)

################################################################################
# Daemon
################################################################################

file(GLOB DAEMON_C "${SOURCE_DIR}/daemon/*.c*")
file(GLOB DAEMON_H "${SOURCE_DIR}/daemon/*.h*")
file(GLOB DAEMON_PLG_H "${SOURCE_DIR}/daemon/plugins/*.h*")
list(APPEND DAEMON_SRC ${DAEMON_C} ${DAEMON_H} ${DAEMON_PLG_H}
"${ROOT_DIR}/third-party/CppSQLite3/CppSQLite3.cpp"
"${ROOT_DIR}/third-party/CppSQLite3/CppSQLite3.h")

add_executable(anDaemon ${DAEMON_SRC})
add_dependencies(anDaemon protoc-rpc-plugin)
target_link_libraries(anDaemon ${LIB} ${PLATFORM_LIB} pb-rpc)

configure_file(${ROOT_DIR}/scripts/startDaemon.sh ${BIN_DIR}/startDaemon.sh)
configure_file(${ROOT_DIR}/scripts/stopDaemon.sh ${BIN_DIR}/stopDaemon.sh)
#add_custom_command(TARGET anDaemon POST_BUILD 
#		   COMMAND ${COMMAND} -E copy_directory "${SOURCE_DIR}/daemon/config" "${BIN_DIR}/daemontest")
#add_custom_command(TARGET anDaemon POST_BUILD 
#		   COMMAND ${COMMAND} -E create_directory "~/.anarchNet")
add_custom_command(TARGET anDaemon POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E create_symlink "${ROOT_DIR}/contrib/anarchnet.conf" "${BIN_DIR}/anarchnet.conf")
################################################################################
# Library
################################################################################

file(GLOB LIBRARY_C "${SOURCE_DIR}/library/*.c*")
file(GLOB LIBRARY_H "${SOURCE_DIR}/library/*.h*")
list(APPEND LIBRARY_SRC ${LIBRARY_C} ${LIBRARY_H})

file(GLOB LIBRARY_JSON_C "third-party/json_spirit/*.c*")
file(GLOB LIBRARY_JSON_H "third-party/json_spirit/*.h*")
list(APPEND LIBRARY_JSON_SRC ${LIBRARY_JSON_C} ${LIBRARY_JSON_H})

add_library(an SHARED ${LIBRARY_SRC} ${LIBRARY_JSON_SRC})
add_dependencies(an pb-rpc protoc-rpc-plugin)
target_link_libraries(an ${LIB} pb-rpc)
################################################################################
# browser
################################################################################

file(GLOB BROWSER_C "browser/*.c*")
file(GLOB BROWSER_H "browser/*.h*")
file(GLOB BROWSER_HTTP_C "browser/http/*.c*")
file(GLOB BROWSER_HTTP_H "browser/http/*.h*")
list(APPEND BROWSER_SRC ${BROWSER_C} ${BROWSER_H} ${BROWSER_HTTP_C} ${BROWSER_HTTP_H})

#add_executable(anBrowser ${BROWSER_SRC})
#add_dependencies(anBrowser liban protoc-rpc-plugin)
#target_link_libraries(anBrowser ${LIB} pb-rpc an)

#add_custom_command(TARGET anBrowser POST_BUILD
#		   COMMAND ${COMMAND} -E create_symlink "${SOURCE_DIR}/browser/www" "${BIN_DIR}/www")
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
file(GLOB T_BASE_C "${SOURCE_DIR}/tests/base/*.c*")
file(GLOB T_BASE_H "${SOURCE_DIR}/tests/base/*.h*")
file(GLOB T_RPC_C "${SOURCE_DIR}/tests/rpc/*.c*")
file(GLOB T_RPC_H "${SOURCE_DIR}/tests/rpc/*.h*")
list(APPEND T_BASE_DIR ${T_BASE_C} ${T_BASE_H})
list(APPEND T_RPC_DIR ${T_RPC_C} ${T_RPC_H} ${PB_DAEMON_FE})

add_executable(TESTbase ${T_MAIN} ${T_BASE_DIR})
add_executable(TESTrpc ${T_MAIN} ${T_RPC_DIR})

add_dependencies(TESTbase anDaemon)
add_dependencies(TESTrpc anDaemon)

target_link_libraries(TESTbase ${LIB} pb-rpc an)
target_link_libraries(TESTrpc ${LIB} pb-rpc an)

enable_testing()
add_test(StartDaemon sh ${BIN_DIR}/startDaemon.sh)
add_test(Base ${BIN_DIR}/TESTbase)
add_test(RPC ${BIN_DIR}/TESTrpc)
add_test(StopDaemon sh ${BIN_DIR}/stopDaemon.sh)

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

#add_subdirectory("${SOURCE_DIR}/modules/")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/bootstrap")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/localstorage")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/remotestorage")
MACRO_OPTIONAL_ADD_SUBDIRECTORY( "${SOURCE_DIR}/modules/session")
