PROJECT(chromium-ipc CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - IPC")

# CMake modules
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DOS_POSIX -DIPC_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT attachment_broker.cc attachment_broker_privileged.cc attachment_broker_unprivileged.cc brokerable_attachment.cc ipc_channel.cc ipc_channel_factory.cc ipc_channel_common.cc ipc_channel_posix.cc ipc_channel_proxy.cc ipc_channel_reader.cc ipc_endpoint.cc ipc_logging.cc ipc_message.cc ipc_message_attachment.cc ipc_message_attachment_set.cc ipc_message_generator.cc ipc_message_utils.cc ipc_platform_file.cc ipc_platform_file_attachment_posix.cc ipc_switches.cc ipc_sync_channel.cc ipc_sync_message.cc ipc_sync_message_filter.cc message_filter.cc message_filter_router.cc placeholder_brokerable_attachment.cc unix_domain_socket_util.cc)


# all files refer to "ipc/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)

ADD_LIBRARY(ipc SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ipc ${link_libraries})

INSTALL(TARGETS ipc
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
