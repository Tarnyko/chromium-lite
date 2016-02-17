PROJECT(chromium-ipc CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - IPC")

# Location of direct dependencies
SET(MOJO_SRC_DIR "${CMAKE_SOURCE_DIR}/../mojo")

# CMake modules
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Common definitions
ADD_DEFINITIONS(-DOS_POSIX -DIPC_IMPLEMENTATION -DIPC_MOJO_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p ipc/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py mojo/client_channel.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/ipc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# core
ADD_LIBRARY(core OBJECT attachment_broker.cc attachment_broker_privileged.cc attachment_broker_unprivileged.cc brokerable_attachment.cc ipc_channel.cc ipc_channel_factory.cc ipc_channel_common.cc ipc_channel_posix.cc ipc_channel_proxy.cc ipc_channel_reader.cc ipc_endpoint.cc ipc_logging.cc ipc_message.cc ipc_message_attachment.cc ipc_message_attachment_set.cc ipc_message_generator.cc ipc_message_utils.cc ipc_platform_file.cc ipc_platform_file_attachment_posix.cc ipc_switches.cc ipc_sync_channel.cc ipc_sync_message.cc ipc_sync_message_filter.cc message_filter.cc message_filter_router.cc placeholder_brokerable_attachment.cc unix_domain_socket_util.cc)

# mojo
ADD_LIBRARY(mojo OBJECT mojo/async_handle_waiter.cc mojo/ipc_channel_mojo.cc mojo/ipc_mojo_bootstrap.cc mojo/ipc_mojo_handle_attachment.cc mojo/ipc_mojo_message_helper.cc mojo/ipc_mojo_param_traits.cc mojo/ipc_message_pipe_reader.cc mojo/scoped_ipc_support.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ipc/mojo/client_channel.mojom.cc)


# all files refer to "ipc/...", so we include ".." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${MOJO_SRC_DIR} ${MOJO_SRC_DIR}/Build ${MOJO_SRC_DIR}/Build/mojo ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/ipc ${CMAKE_SOURCE_DIR}/..)
# TODO : "${MOJO_SRC_DIR}/Build" here is a HACK ! remove this when we use .pc files

ADD_LIBRARY(ipc SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:mojo> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ipc ${link_libraries})

INSTALL(TARGETS ipc
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
