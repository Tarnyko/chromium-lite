PROJECT(chromium-mojo CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Mojo")

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
ADD_DEFINITIONS(-DMOJO_COMMON_IMPLEMENTATION -DMOJO_ENVIRONMENT_IMPL_IMPLEMENTATION -DMOJO_MESSAGE_PUMP_IMPLEMENTATION -DMOJO_SYSTEM_IMPL_IMPLEMENTATION -DMOJO_SYSTEM_IMPLEMENTATION -DMOJO_USE_SYSTEM_IMPL)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .TMPL -> .ZIP (bytecode)

MESSAGE(STATUS "Generating .zip files from .tmpl...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs precompile -o ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .MOJOM -> .CC,.H (bindings)

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/bindings/interface_control_messages.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/bindings/pipe_control_messages.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate common/time.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/bindings/interface_control_messages.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/bindings/pipe_control_messages.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate common/time.mojom -d . -I . -o ${CMAKE_BINARY_DIR}/mojo --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# common
ADD_LIBRARY(common OBJECT common/data_pipe_utils.cc)

# system
ADD_LIBRARY(system OBJECT public/c/system/thunks.cc public/cpp/system/platform_handle.cc  public/cpp/system/watcher.cc)

# bindings
ADD_LIBRARY(bindings OBJECT public/cpp/bindings/lib/array_internal.cc public/cpp/bindings/lib/associated_group.cc public/cpp/bindings/lib/associated_group_controller.cc public/cpp/bindings/lib/connector.cc public/cpp/bindings/lib/control_message_handler.cc public/cpp/bindings/lib/control_message_proxy.cc public/cpp/bindings/lib/filter_chain.cc public/cpp/bindings/lib/fixed_buffer.cc public/cpp/bindings/lib/interface_endpoint_client.cc public/cpp/bindings/lib/message.cc public/cpp/bindings/lib/message_buffer.cc public/cpp/bindings/lib/message_builder.cc public/cpp/bindings/lib/message_header_validator.cc public/cpp/bindings/lib/multiplex_router.cc public/cpp/bindings/lib/native_struct.cc public/cpp/bindings/lib/native_struct_data.cc public/cpp/bindings/lib/native_struct_serialization.cc public/cpp/bindings/lib/pipe_control_message_handler.cc public/cpp/bindings/lib/pipe_control_message_proxy.cc public/cpp/bindings/lib/scoped_interface_endpoint_handle.cc public/cpp/bindings/lib/serialization_context.cc public/cpp/bindings/lib/sync_call_restrictions.cc public/cpp/bindings/lib/sync_handle_registry.cc public/cpp/bindings/lib/sync_handle_watcher.cc public/cpp/bindings/lib/validation_context.cc public/cpp/bindings/lib/validation_errors.cc public/cpp/bindings/lib/validation_util.cc)

# edk
ADD_LIBRARY(edk OBJECT edk/embedder/embedder.cc edk/embedder/entrypoints.cc edk/embedder/platform_channel_pair.cc edk/embedder/platform_channel_pair_posix.cc edk/embedder/platform_channel_utils_posix.cc edk/embedder/platform_handle.cc edk/embedder/platform_handle_utils_posix.cc edk/embedder/platform_shared_buffer.cc edk/embedder/scoped_ipc_support.cc edk/js/handle.cc edk/system/awakable_list.cc edk/system/broker_host.cc edk/system/broker_posix.cc edk/system/channel.cc edk/system/channel_posix.cc edk/system/configuration.cc edk/system/core.cc edk/system/data_pipe_consumer_dispatcher.cc edk/system/data_pipe_control_message.cc edk/system/data_pipe_producer_dispatcher.cc edk/system/dispatcher.cc edk/system/handle_table.cc edk/system/mapping_table.cc edk/system/message_for_transit.cc edk/system/message_pipe_dispatcher.cc edk/system/node_channel.cc edk/system/node_controller.cc edk/system/platform_handle_dispatcher.cc edk/system/ports_message.cc edk/system/request_context.cc edk/system/shared_buffer_dispatcher.cc edk/system/wait_set_dispatcher.cc edk/system/waiter.cc edk/system/watcher.cc edk/system/watcher_set.cc edk/system/ports/event.cc edk/system/ports/message.cc edk/system/ports/message_queue.cc edk/system/ports/name.cc edk/system/ports/node.cc edk/system/ports/port.cc edk/system/ports/port_ref.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/interface_control_messages.mojom.cc ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/interface_control_messages.mojom-shared.cc ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/pipe_control_messages.mojom.cc ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/pipe_control_messages.mojom-shared.cc ${CMAKE_BINARY_DIR}/mojo/common/time.mojom.cc ${CMAKE_BINARY_DIR}/mojo/common/time.mojom-shared.cc)


# all files refer to "mojo/...", so we include ".." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
# we include Webkit's "wtf" headers, required by "string_traits_wtf.h"
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/mojo ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../third_party/WebKit/Source)

ADD_LIBRARY(mojo SHARED $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:system> $<TARGET_OBJECTS:bindings> $<TARGET_OBJECTS:edk> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(mojo ${LINK_LIBRARIES})

INSTALL(TARGETS mojo
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
