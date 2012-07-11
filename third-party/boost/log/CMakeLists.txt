PROJECT( boost-log )

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( BUILD_SHARED_LIBS YES )

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE( Boost COMPONENTS thread date_time system filesystem) 

INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/../.. )

SET( SOURCES 
    libs/log/src/attribute_name.cpp
    libs/log/src/attribute_set.cpp
    libs/log/src/attribute_values_view.cpp
    libs/log/src/code_conversion.cpp
    libs/log/src/core.cpp
    libs/log/src/record_ostream.cpp
    libs/log/src/severity_level.cpp
    libs/log/src/global_logger_storage.cpp
    libs/log/src/named_scope.cpp
    libs/log/src/process_id.cpp
    libs/log/src/timer.cpp
    libs/log/src/exceptions.cpp

    libs/log/src/text_ostream_backend.cpp
    libs/log/src/text_file_backend.cpp
    libs/log/src/syslog_backend.cpp
    libs/log/src/thread_specific.cpp
    libs/log/src/once_block.cpp
    libs/log/src/threadsafe_queue.cpp
    libs/log/src/trivial.cpp
    libs/log/src/parser_utils.cpp
    libs/log/src/init_from_stream.cpp
    libs/log/src/init_from_settings.cpp
    libs/log/src/filter_parser.cpp
    libs/log/src/formatter_parser.cpp
    libs/log/src/default_filter_factory.cpp
libs/log/src/default_sink.cpp

libs/log/src/event.cpp
libs/log/src/thread_id.cpp
libs/log/src/timestamp.cpp

)

if( WIN32 )
 SET (PF_SOURCES 
libs/log/src/debug_output_backend.cpp
)
endif()


add_library(boost_log ${SOURCES} ${PF_SOURCES})
target_link_libraries(boost_log rt)
