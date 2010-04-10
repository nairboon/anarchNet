#
# anarchNet 
#

project(anarchNet)
cmake_minimum_required(VERSION 2.6)

if (CMAKE_BUILD_TYPE STREQUAL "")
   set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
endif ()

if(APPLE AND CMAKE_GENERATOR MATCHES "Xcode")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
	set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
	set(BIN_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
###################################################################################################
# Dependencies
###################################################################################################

find_library(GLOG NAMES libglog.a glog REQUIRED)
find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)

set(Boost_ADDITIONAL_VERSIONS "1.42")
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.42 COMPONENTS program_options system thread filesystem  REQUIRED)

include_directories( 
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/daemon"
	"${CMAKE_SOURCE_DIR}/library"
	"/usr/local/include"
	${Boost_INCLUDE_DIRS} 
	${GTEST_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/daemon/protocol"
	"${CMAKE_SOURCE_DIR}/daemon/protocol/frontend"
	"${CMAKE_SOURCE_DIR}/daemon/protocol/backend"
	"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/rpc"
	"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin"
	"${CMAKE_SOURCE_DIR}/third-party/pugg/include" )
set( LIB
	${Boost_LIBRARIES}
	${GLOG}
	${GCOV}
	${GTEST_LIBRARIES}
	${PROTOBUF_LIBRARIES}
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
set(RPC_PLG_SRC "${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/file_generator.cc"
		"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/helpers.cc"
		"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/plugin.cc"
		"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/service_generator.cc"
		"${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/service.cc")
add_executable(protoc-gen-rpc ${RPC_PLG_SRC})
target_link_libraries(protoc-gen-rpc ${PROTOBUF_LIBRARIES} ${PROTOBUF_PROTOC_LIBRARY} ${PLATFORM_LIB})

### generate proto files ###


set(PROTO_FILES "${CMAKE_SOURCE_DIR}/daemon/protocol/Control.proto"
       		)
generate_protobuf_files(${PROTO_FILES})
set(PB_DAEMON_FE ${SRCS} ${HDRS})

set(PROTO_FILES "${CMAKE_SOURCE_DIR}/daemon/protocol/Util.proto"
       		"${CMAKE_SOURCE_DIR}/daemon/protocol/protorpc.proto")
generate_protobuf_files(${PROTO_FILES})
set(PB_DAEMON_G ${SRCS} ${HDRS})

file(GLOB PB-RPC_C "${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/rpc/*.c*")
file(GLOB PB-RPC_H "${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/rpc/*.h*")
list(APPEND PB-RPC_SRC ${PB-RPC_C} ${PB-RPC_H}
		   "${CMAKE_SOURCE_DIR}/daemon/protocol/protorpc.pb.cc"
		   "${CMAKE_SOURCE_DIR}/daemon/protocol/Util.pb.cc"
		   "${CMAKE_SOURCE_DIR}/third-party/protobuf-jerpc/plugin/service.cc"
)

add_library(pb-rpc ${PB-RPC_SRC})

################################################################################
# Daemon
################################################################################

file(GLOB DAEMON_C "daemon/*.c*")
file(GLOB DAEMON_H "daemon/*.h*")
list(APPEND DAEMON_SRC ${DAEMON_C} ${DAEMON_H} ${PB_DAEMON_FE}
"${CMAKE_SOURCE_DIR}/third-party/pugg/include/puggDriver.h"
"${CMAKE_SOURCE_DIR}/third-party/pugg/include/puggKernel.h"
"${CMAKE_SOURCE_DIR}/third-party/pugg/include/puggPlugin.h"
"${CMAKE_SOURCE_DIR}/third-party/pugg/include/puggServer.h")

add_executable(anDaemon ${DAEMON_SRC})
add_dependencies(anDaemon protoc-rpc-plugin)
target_link_libraries(anDaemon ${LIB} ${PLATFORM_LIB} pb-rpc)

configure_file(${CMAKE_SOURCE_DIR}/scripts/startDaemon.sh ${BIN_DIR}/startDaemon.sh)
configure_file(${CMAKE_SOURCE_DIR}/scripts/stopDaemon.sh ${BIN_DIR}/stopDaemon.sh)
add_custom_command(TARGET anDaemon POST_BUILD 
		   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/daemon/config" "${BIN_DIR}/daemontest")
add_custom_command(TARGET anDaemon POST_BUILD
		   COMMAND ${CMAKE_COMMAND} -E create_symlink "${BIN_DIR}/daemontest" "~/.anarchNet")
################################################################################
# Library
################################################################################

file(GLOB LIBRARY_C "library/*.c*")
file(GLOB LIBRARY_H "library/*.h*")
list(APPEND LIBRARY_SRC ${LIBRARY_C} ${LIBRARY_H})

add_library(liban ${LIBRARY_SRC})
add_dependencies(liban protoc-rpc-plugin)
target_link_libraries(liban ${LIB} pb-rpc)
###################################################################################################
# Python library search (style check)                                                             #
###################################################################################################
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
file(GLOB T_MAIN  "tests/main.cc")
file(GLOB T_BASE_C "tests/base/*.c*")
file(GLOB T_BASE_H "tests/base/*.h*")
file(GLOB T_RPC_C "tests/rpc/*.c*")
file(GLOB T_RPC_H "tests/rpc/*.h*")
list(APPEND T_BASE_DIR ${T_BASE_C} ${T_BASE_H})
list(APPEND T_RPC_DIR ${T_RPC_C} ${T_RPC_H} ${PB_DAEMON_FE})

add_executable(TESTbase ${T_MAIN} ${T_BASE_DIR})
add_executable(TESTrpc ${T_MAIN} ${T_RPC_DIR})

add_dependencies(TESTbase anDaemon)
add_dependencies(TESTrpc anDaemon)

target_link_libraries(TESTbase ${LIB})
target_link_libraries(TESTrpc ${LIB} pb-rpc liban)

enable_testing()
add_test(StartDaemon sh ${BIN_DIR}/startDaemon.sh)
add_test(Base ${BIN_DIR}/TESTbase)
add_test(RPC ${BIN_DIR}/TESTrpc)
add_test(StopDaemon sh ${BIN_DIR}/stopDaemon.sh)

if (PYTHONINTERP_FOUND)
  set(CPPLINTOPS "--verbose=4" "--filter=-whitespace/tab,-whitespace/end_of_line")
  if (UNIX)
    add_test(STYLE_CHECK_DAEMON ${CMAKE_SOURCE_DIR}/cpplint.py ${CPPLINTOPS} ${DAEMON_SRC})
    set_property(TEST STYLE_CHECK_DAEMON PROPERTY LABELS Functional Daemon CodingStyle)
   # ADD_TEST(STYLE_CHECK_LIBRARY ${MAIDSAFE_SOURCE_DIR}/cpplint.py ${M_BASE_DIR} ${T_BASE_DIR})
   # SET_PROPERTY(TEST STYLE_CHECK_LIBRARY PROPERTY LABELS Functional Library CodingStyle)
  else ()
    add_test(STYLE_CHECK finderrorsall.bat)
    set_property(TEST STYLE_CHECK PROPERTY LABELS Functional CodingStyle)
  endif ()
endif ()
