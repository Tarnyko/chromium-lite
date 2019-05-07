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


# PRE-GENERATION .TMPL -> .ZIP (bytecode)

MESSAGE(STATUS "Generating .zip files from .tmpl...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs precompile -o ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p ipc/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate ipc.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/ipc --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate ipc.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/ipc --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# core
ADD_LIBRARY(core OBJECT ipc_channel.cc ipc_channel_factory.cc ipc_channel_common.cc ipc_channel_proxy.cc ipc_channel_reader.cc ipc_logging.cc ipc_message.cc ipc_message_attachment.cc ipc_message_attachment_set.cc ipc_message_utils.cc ipc_platform_file.cc ipc_platform_file_attachment_posix.cc ipc_sync_channel.cc ipc_sync_message.cc ipc_sync_message_filter.cc message_filter.cc message_filter_router.cc)

# mojo
ADD_LIBRARY(mojo OBJECT ipc_channel_mojo.cc ipc_mojo_bootstrap.cc ipc_mojo_handle_attachment.cc ipc_mojo_message_helper.cc ipc_mojo_param_traits.cc ipc_message_pipe_reader.cc mojo_event.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ipc/ipc.mojom.cc ${CMAKE_BINARY_DIR}/ipc/ipc.mojom-shared.cc)


# all files refer to "ipc/...", so we include ".." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${MOJO_SRC_DIR} ${MOJO_SRC_DIR}/Build ${MOJO_SRC_DIR}/Build/mojo ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/ipc ${CMAKE_SOURCE_DIR}/..)
# TODO : "${MOJO_SRC_DIR}/Build" here is a HACK ! remove this when we use .pc files

ADD_LIBRARY(ipc SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:mojo> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ipc ${LINK_LIBRARIES})

INSTALL(TARGETS ipc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
