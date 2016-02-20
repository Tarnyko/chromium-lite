PROJECT(chromium-mojo CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Mojo")

# Location of third-party extensions
SET(MOJO_EDK_EXT_DIR "${CMAKE_SOURCE_DIR}/../third_party/mojo/src/mojo/edk")

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


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py public/interfaces/bindings/interface_control_messages.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py public/interfaces/bindings/pipe_control_messages.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py application/public/interfaces/service_provider.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py application/public/interfaces/shell.mojom --use_bundled_pylibs -d . -I.. -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py application/public/interfaces/application.mojom --use_bundled_pylibs -d . -I.. -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py application/public/interfaces/application_manager.mojom --use_bundled_pylibs -d . -I.. -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py application/public/interfaces/content_handler.mojom --use_bundled_pylibs -d . -I.. -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/http_message.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/network_error.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/url_loader.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/url_loader_factory.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/network_service.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/cookie_store.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/http_server.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/http_connection.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/web_socket.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/net_address.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/tcp_bound_socket.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/tcp_connected_socket.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/tcp_server_socket.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/network/public/interfaces/udp_socket.mojom --use_bundled_pylibs -d . -I./services -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python public/tools/bindings/mojom_bindings_generator.py services/updater/updater.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/mojo WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})



# SOURCES FILE LIST

# common
ADD_LIBRARY(common OBJECT common/common_type_converters.cc common/data_pipe_file_utils.cc common/data_pipe_utils.cc common/url_type_converters.cc)

# environment
ADD_LIBRARY(environment OBJECT public/cpp/environment/lib/async_waiter.cc public/cpp/environment/lib/logging.cc public/cpp/environment/lib/scoped_task_tracking.cc environment/default_async_waiter_impl.cc environment/default_logger_impl.cc environment/default_run_loop_impl.cc environment/default_task_tracker_impl.cc environment/environment.cc)

# message_pump
ADD_LIBRARY(message_pump OBJECT message_pump/handle_watcher.cc message_pump/time_helper.cc message_pump/message_pump_mojo.cc)

# bindings
ADD_LIBRARY(bindings OBJECT public/cpp/bindings/lib/array_internal.cc public/cpp/bindings/lib/associated_group.cc public/cpp/bindings/lib/bindings_serialization.cc public/cpp/bindings/lib/bounds_checker.cc public/cpp/bindings/lib/connector.cc public/cpp/bindings/lib/control_message_handler.cc public/cpp/bindings/lib/control_message_proxy.cc public/cpp/bindings/lib/filter_chain.cc public/cpp/bindings/lib/fixed_buffer.cc public/cpp/bindings/lib/interface_endpoint_client.cc public/cpp/bindings/lib/message.cc public/cpp/bindings/lib/message_builder.cc public/cpp/bindings/lib/message_filter.cc public/cpp/bindings/lib/message_header_validator.cc public/cpp/bindings/lib/multiplex_router.cc public/cpp/bindings/lib/no_interface.cc public/cpp/bindings/lib/pipe_control_message_handler.cc public/cpp/bindings/lib/pipe_control_message_proxy.cc public/cpp/bindings/lib/router.cc public/cpp/bindings/lib/scoped_interface_endpoint_handle.cc public/cpp/bindings/lib/string_serialization.cc public/cpp/bindings/lib/validation_errors.cc public/cpp/bindings/lib/validation_util.cc)

# application
ADD_LIBRARY(application OBJECT application/public/cpp/initialize_base_and_icu.cc application/public/cpp/lib/app_lifetime_helper.cc application/public/cpp/lib/application_delegate.cc application/public/cpp/lib/application_impl.cc application/public/cpp/lib/application_runner.cc application/public/cpp/lib/service_connector_registry.cc application/public/cpp/lib/service_provider_impl.cc application/public/cpp/lib/service_registry.cc)

# fetcher
ADD_LIBRARY(fetcher OBJECT fetcher/about_fetcher.cc fetcher/data_fetcher.cc fetcher/local_fetcher.cc fetcher/network_fetcher.cc fetcher/update_fetcher.cc fetcher/switches.cc fetcher/url_resolver.cc package_manager/content_handler_connection.cc package_manager/package_manager_impl.cc)

# shell
ADD_LIBRARY(shell OBJECT shell/application_instance.cc shell/application_manager.cc shell/capability_filter.cc shell/connect_to_application_params.cc shell/connect_util.cc shell/data_pipe_peek.cc shell/fetcher.cc shell/identity.cc shell/query_util.cc shell/shell_application_delegate.cc shell/shell_application_loader.cc shell/static_application_loader.cc shell/switches.cc util/filename_util.cc)

# converters
ADD_LIBRARY(converters OBJECT converters/network/network_type_converters.cc)

# edk
ADD_LIBRARY(edk OBJECT edk/embedder/embedder.cc edk/embedder/entrypoints.cc edk/embedder/platform_channel_pair.cc edk/embedder/platform_channel_pair_posix.cc edk/embedder/platform_channel_utils_posix.cc edk/embedder/platform_handle.cc edk/embedder/platform_handle_utils_posix.cc edk/embedder/simple_platform_shared_buffer.cc edk/embedder/simple_platform_shared_buffer_posix.cc edk/embedder/simple_platform_support.cc edk/embedder/test_embedder.cc edk/system/awakable_list.cc edk/system/async_waiter.cc edk/system/broker_state.cc edk/system/child_broker.cc edk/system/child_broker_host.cc edk/system/configuration.cc edk/system/core.cc edk/system/data_pipe.cc edk/system/data_pipe_consumer_dispatcher.cc edk/system/data_pipe_producer_dispatcher.cc edk/system/dispatcher.cc edk/system/handle_table.cc edk/system/mapping_table.cc edk/system/message_in_transit.cc edk/system/message_in_transit_queue.cc edk/system/message_pipe_dispatcher.cc edk/system/platform_handle_dispatcher.cc edk/system/raw_channel.cc edk/system/raw_channel_posix.cc edk/system/routed_raw_channel.cc edk/system/shared_buffer_dispatcher.cc edk/system/simple_dispatcher.cc edk/system/transport_data.cc edk/system/wait_set_dispatcher.cc edk/system/waiter.cc)

# edk_ext
ADD_LIBRARY(edk_ext OBJECT ${MOJO_EDK_EXT_DIR}/embedder/embedder.cc ${MOJO_EDK_EXT_DIR}/embedder/entrypoints.cc ${MOJO_EDK_EXT_DIR}/embedder/platform_channel_pair.cc ${MOJO_EDK_EXT_DIR}/embedder/platform_channel_pair_posix.cc ${MOJO_EDK_EXT_DIR}/embedder/platform_channel_utils_posix.cc ${MOJO_EDK_EXT_DIR}/embedder/platform_handle.cc ${MOJO_EDK_EXT_DIR}/embedder/platform_handle_utils_posix.cc ${MOJO_EDK_EXT_DIR}/embedder/simple_platform_shared_buffer.cc ${MOJO_EDK_EXT_DIR}/embedder/simple_platform_shared_buffer_posix.cc ${MOJO_EDK_EXT_DIR}/embedder/simple_platform_support.cc ${MOJO_EDK_EXT_DIR}/embedder/test_embedder.cc ${MOJO_EDK_EXT_DIR}/system/async_waiter.cc  ${MOJO_EDK_EXT_DIR}/system/awakable_list.cc ${MOJO_EDK_EXT_DIR}/system/channel.cc ${MOJO_EDK_EXT_DIR}/system/channel_endpoint.cc ${MOJO_EDK_EXT_DIR}/system/channel_endpoint_id.cc ${MOJO_EDK_EXT_DIR}/system/channel_manager.cc ${MOJO_EDK_EXT_DIR}/system/configuration.cc ${MOJO_EDK_EXT_DIR}/system/connection_manager.cc ${MOJO_EDK_EXT_DIR}/system/core.cc ${MOJO_EDK_EXT_DIR}/system/data_pipe.cc ${MOJO_EDK_EXT_DIR}/system/data_pipe_consumer_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/data_pipe_impl.cc ${MOJO_EDK_EXT_DIR}/system/data_pipe_producer_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/endpoint_relayer.cc ${MOJO_EDK_EXT_DIR}/system/handle_table.cc ${MOJO_EDK_EXT_DIR}/system/incoming_endpoint.cc ${MOJO_EDK_EXT_DIR}/system/ipc_support.cc ${MOJO_EDK_EXT_DIR}/system/local_data_pipe_impl.cc ${MOJO_EDK_EXT_DIR}/system/local_message_pipe_endpoint.cc ${MOJO_EDK_EXT_DIR}/system/mapping_table.cc ${MOJO_EDK_EXT_DIR}/system/master_connection_manager.cc ${MOJO_EDK_EXT_DIR}/system/memory.cc ${MOJO_EDK_EXT_DIR}/system/message_in_transit.cc ${MOJO_EDK_EXT_DIR}/system/message_in_transit_queue.cc ${MOJO_EDK_EXT_DIR}/system/message_pipe.cc ${MOJO_EDK_EXT_DIR}/system/message_pipe_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/message_pipe_endpoint.cc ${MOJO_EDK_EXT_DIR}/system/mutex.cc ${MOJO_EDK_EXT_DIR}/system/platform_handle_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/proxy_message_pipe_endpoint.cc ${MOJO_EDK_EXT_DIR}/system/remote_consumer_data_pipe_impl.cc ${MOJO_EDK_EXT_DIR}/system/raw_channel.cc ${MOJO_EDK_EXT_DIR}/system/raw_channel_posix.cc ${MOJO_EDK_EXT_DIR}/system/remote_producer_data_pipe_impl.cc  ${MOJO_EDK_EXT_DIR}/system/shared_buffer_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/simple_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/slave_connection_manager.cc ${MOJO_EDK_EXT_DIR}/system/transport_data.cc ${MOJO_EDK_EXT_DIR}/system/unique_identifier.cc ${MOJO_EDK_EXT_DIR}/system/wait_set_dispatcher.cc ${MOJO_EDK_EXT_DIR}/system/waiter.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/interface_control_messages.mojom.cc ${CMAKE_BINARY_DIR}/mojo/public/interfaces/bindings/pipe_control_messages.mojom.cc ${CMAKE_BINARY_DIR}/mojo/application/public/interfaces/service_provider.mojom.cc ${CMAKE_BINARY_DIR}/mojo/application/public/interfaces/shell.mojom.cc ${CMAKE_BINARY_DIR}/mojo/application/public/interfaces/application.mojom.cc ${CMAKE_BINARY_DIR}/mojo/application/public/interfaces/application_manager.mojom.cc ${CMAKE_BINARY_DIR}/mojo/application/public/interfaces/content_handler.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/http_message.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/network_error.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/url_loader.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/url_loader_factory.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/network_service.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/cookie_store.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/http_server.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/http_connection.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/web_socket.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/net_address.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/tcp_bound_socket.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/tcp_connected_socket.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/tcp_server_socket.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/network/public/interfaces/udp_socket.mojom.cc ${CMAKE_BINARY_DIR}/mojo/services/updater/updater.mojom.cc)


# all files refer to "mojo/...", so we include ".." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/mojo ${CMAKE_SOURCE_DIR}/..)

ADD_LIBRARY(mojo SHARED $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:environment> $<TARGET_OBJECTS:message_pump> $<TARGET_OBJECTS:bindings> $<TARGET_OBJECTS:application> $<TARGET_OBJECTS:fetcher> $<TARGET_OBJECTS:shell> $<TARGET_OBJECTS:converters> $<TARGET_OBJECTS:edk> $<TARGET_OBJECTS:edk_ext> $<TARGET_OBJECTS:generated> public/js/constants.cc)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(mojo ${link_libraries})

INSTALL(TARGETS mojo
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
