PROJECT(chromium-ppapi CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Pepper API")

# Location of direct dependencies
SET(GPU_SRC_DIR "${CMAKE_SOURCE_DIR}/../gpu")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Location of build tools dir (required for generating "features.h")
SET(BUILD_TOOLS_DIR "${CMAKE_SOURCE_DIR}/../build")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DPPAPI_SHARED_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION FEATURES.H

MESSAGE(STATUS "Generating features.h...")

EXECUTE_PROCESS(COMMAND mkdir -p ppapi/features WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND sh -c "echo --flags ENABLE_PEPPER_CDMS=1 ENABLE_PLUGINS=1 > buildflag_header.rsp" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND python ${BUILD_TOOLS_DIR}/write_buildflag_header.py --output ppapi/features/features.h --rulename //ppapi/features:features --gen-dir ${CMAKE_BINARY_DIR} --definitions ${CMAKE_BINARY_DIR}/buildflag_header.rsp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# cpp
ADD_LIBRARY(cpp OBJECT cpp/ppp_entrypoints.cc cpp/array_output.cc cpp/audio.cc cpp/audio_buffer.cc cpp/audio_config.cc cpp/audio_encoder.cc cpp/compositor.cc cpp/compositor_layer.cc cpp/core.cc cpp/directory_entry.cc cpp/file_io.cc cpp/file_ref.cc cpp/file_system.cc cpp/fullscreen.cc cpp/graphics_2d.cc cpp/graphics_3d.cc cpp/graphics_3d_client.cc cpp/host_resolver.cc cpp/image_data.cc cpp/input_event.cc cpp/instance.cc cpp/instance_handle.cc cpp/media_stream_audio_track.cc cpp/media_stream_video_track.cc cpp/message_loop.cc cpp/module.cc cpp/mouse_cursor.cc cpp/mouse_lock.cc cpp/net_address.cc cpp/network_list.cc cpp/network_monitor.cc cpp/network_proxy.cc cpp/rect.cc cpp/resource.cc cpp/tcp_socket.cc cpp/text_input_controller.cc cpp/udp_socket.cc cpp/url_loader.cc cpp/url_request_info.cc cpp/url_response_info.cc cpp/var.cc cpp/var_array.cc cpp/var_array_buffer.cc cpp/var_dictionary.cc cpp/video_decoder.cc cpp/video_encoder.cc cpp/video_frame.cc cpp/view.cc cpp/vpn_provider.cc cpp/websocket.cc cpp/dev/audio_input_dev.cc cpp/dev/buffer_dev.cc cpp/dev/crypto_dev.cc cpp/dev/cursor_control_dev.cc cpp/dev/device_ref_dev.cc cpp/dev/file_chooser_dev.cc cpp/dev/ime_input_event_dev.cc cpp/dev/memory_dev.cc cpp/dev/printing_dev.cc cpp/dev/text_input_dev.cc cpp/dev/truetype_font_dev.cc cpp/dev/url_util_dev.cc cpp/dev/video_capture_client_dev.cc cpp/dev/video_capture_dev.cc cpp/dev/video_decoder_client_dev.cc cpp/dev/video_decoder_dev.cc cpp/dev/view_dev.cc cpp/dev/scriptable_object_deprecated.cc cpp/private/camera_capabilities_private.cc cpp/private/camera_device_private.cc cpp/private/content_decryptor_private.cc cpp/private/ext_crx_file_system_private.cc cpp/private/file_io_private.cc cpp/private/find_private.cc cpp/private/flash.cc cpp/private/flash_clipboard.cc cpp/private/flash_device_id.cc cpp/private/flash_drm.cc cpp/private/flash_file.cc cpp/private/flash_font_file.cc cpp/private/flash_fullscreen.cc cpp/private/flash_menu.cc cpp/private/flash_message_loop.cc cpp/private/host_resolver_private.cc cpp/private/instance_private.cc cpp/private/isolated_file_system_private.cc cpp/private/net_address_private.cc cpp/private/output_protection_private.cc cpp/private/pass_file_handle.cc cpp/private/pdf.cc cpp/private/platform_verification.cc cpp/private/tcp_server_socket_private.cc cpp/private/tcp_socket_private.cc cpp/private/udp_socket_private.cc cpp/private/uma_private.cc cpp/private/var_private.cc cpp/private/video_destination_private.cc cpp/private/video_frame_private.cc cpp/private/video_source_private.cc cpp/private/x509_certificate_private.cc cpp/trusted/browser_font_trusted.cc cpp/trusted/file_chooser_trusted.cc utility/graphics/paint_aggregator.cc utility/graphics/paint_manager.cc utility/threading/lock.cc utility/threading/simple_thread.cc utility/websocket/websocket_api.cc )

# thunk
ADD_LIBRARY(thunk OBJECT thunk/enter.cc thunk/ppb_audio_buffer_thunk.cc thunk/ppb_audio_config_thunk.cc thunk/ppb_audio_encoder_thunk.cc thunk/ppb_audio_input_dev_thunk.cc thunk/ppb_audio_thunk.cc thunk/ppb_broker_thunk.cc thunk/ppb_browser_font_trusted_thunk.cc thunk/ppb_buffer_thunk.cc thunk/ppb_camera_capabilities_private_thunk.cc thunk/ppb_camera_device_private_thunk.cc thunk/ppb_char_set_thunk.cc thunk/ppb_compositor_layer_thunk.cc thunk/ppb_compositor_thunk.cc thunk/ppb_console_thunk.cc thunk/ppb_content_decryptor_private_thunk.cc thunk/ppb_cursor_control_thunk.cc thunk/ppb_device_ref_dev_thunk.cc thunk/ppb_ext_crx_file_system_private_thunk.cc thunk/ppb_file_chooser_dev_thunk.cc thunk/ppb_file_chooser_trusted_thunk.cc thunk/ppb_file_io_private_thunk.cc thunk/ppb_file_io_thunk.cc thunk/ppb_file_ref_thunk.cc thunk/ppb_file_system_thunk.cc thunk/ppb_pdf_thunk.cc thunk/ppb_find_private_thunk.cc thunk/ppb_flash_clipboard_thunk.cc thunk/ppb_flash_device_id_thunk.cc thunk/ppb_flash_drm_thunk.cc thunk/ppb_flash_file_fileref_thunk.cc thunk/ppb_flash_file_modulelocal_thunk.cc thunk/ppb_flash_font_file_thunk.cc thunk/ppb_flash_fullscreen_thunk.cc thunk/ppb_flash_menu_thunk.cc thunk/ppb_flash_message_loop_thunk.cc thunk/ppb_flash_print_thunk.cc thunk/ppb_flash_thunk.cc thunk/ppb_fullscreen_thunk.cc thunk/ppb_gamepad_thunk.cc thunk/ppb_gles_chromium_texture_mapping_thunk.cc thunk/ppb_graphics_2d_thunk.cc thunk/ppb_graphics_3d_thunk.cc thunk/ppb_host_resolver_private_thunk.cc thunk/ppb_host_resolver_thunk.cc thunk/ppb_image_data_thunk.cc thunk/ppb_input_event_thunk.cc thunk/ppb_instance_private_thunk.cc thunk/ppb_instance_thunk.cc thunk/ppb_isolated_file_system_private_thunk.cc thunk/ppb_media_stream_audio_track_thunk.cc thunk/ppb_media_stream_video_track_thunk.cc thunk/ppb_messaging_thunk.cc thunk/ppb_mouse_cursor_thunk.cc thunk/ppb_mouse_lock_thunk.cc thunk/ppb_net_address_thunk.cc thunk/ppb_network_list_thunk.cc thunk/ppb_network_monitor_thunk.cc thunk/ppb_network_proxy_thunk.cc thunk/ppb_output_protection_private_thunk.cc thunk/ppb_platform_verification_private_thunk.cc thunk/ppb_printing_dev_thunk.cc thunk/ppb_tcp_server_socket_private_thunk.cc thunk/ppb_tcp_socket_private_thunk.cc thunk/ppb_tcp_socket_thunk.cc thunk/ppb_text_input_thunk.cc thunk/ppb_truetype_font_dev_thunk.cc thunk/ppb_udp_socket_private_thunk.cc thunk/ppb_udp_socket_thunk.cc thunk/ppb_uma_private_thunk.cc thunk/ppb_url_loader_thunk.cc thunk/ppb_url_request_info_thunk.cc thunk/ppb_url_response_info_thunk.cc thunk/ppb_url_util_thunk.cc thunk/ppb_var_array_thunk.cc thunk/ppb_var_dictionary_thunk.cc thunk/ppb_video_capture_thunk.cc thunk/ppb_video_decoder_thunk.cc thunk/ppb_video_decoder_dev_thunk.cc thunk/ppb_video_destination_private_thunk.cc thunk/ppb_video_encoder_thunk.cc thunk/ppb_video_frame_thunk.cc thunk/ppb_video_source_private_thunk.cc thunk/ppb_view_dev_thunk.cc thunk/ppb_vpn_provider_thunk.cc thunk/ppb_x509_certificate_private_thunk.cc)

# sharedimpl
ADD_LIBRARY(sharedimpl OBJECT shared_impl/array_var.cc shared_impl/array_writer.cc shared_impl/callback_tracker.cc shared_impl/compositor_layer_data.cc shared_impl/dictionary_var.cc shared_impl/file_growth.cc shared_impl/file_io_state_manager.cc shared_impl/file_path.cc shared_impl/file_ref_create_info.cc shared_impl/file_ref_util.cc shared_impl/file_system_util.cc shared_impl/file_type_conversion.cc shared_impl/flash_clipboard_format_registry.cc shared_impl/host_resource.cc shared_impl/id_assignment.cc shared_impl/media_stream_audio_track_shared.cc shared_impl/media_stream_buffer_manager.cc shared_impl/media_stream_video_track_shared.cc shared_impl/platform_file.cc shared_impl/ppapi_globals.cc shared_impl/ppapi_nacl_plugin_args.cc shared_impl/ppapi_permissions.cc shared_impl/ppapi_preferences.cc shared_impl/ppapi_switches.cc shared_impl/ppb_audio_config_shared.cc shared_impl/ppb_audio_shared.cc shared_impl/ppb_crypto_shared.cc shared_impl/ppb_device_ref_shared.cc shared_impl/ppb_gamepad_shared.cc shared_impl/ppb_graphics_3d_shared.cc shared_impl/ppb_image_data_shared.cc shared_impl/ppb_input_event_shared.cc shared_impl/ppb_instance_shared.cc shared_impl/ppb_memory_shared.cc shared_impl/ppb_message_loop_shared.cc shared_impl/ppb_opengles2_shared.cc shared_impl/ppb_tcp_socket_shared.cc shared_impl/ppb_trace_event_impl.cc shared_impl/ppb_url_util_shared.cc shared_impl/ppb_var_shared.cc shared_impl/ppb_video_decoder_shared.cc shared_impl/ppb_view_shared.cc shared_impl/ppp_instance_combined.cc shared_impl/proxy_lock.cc shared_impl/resource.cc shared_impl/resource_tracker.cc shared_impl/resource_var.cc shared_impl/scoped_pp_resource.cc shared_impl/scoped_pp_var.cc shared_impl/socket_option_data.cc shared_impl/thread_aware_callback.cc shared_impl/time_conversion.cc shared_impl/tracked_callback.cc shared_impl/url_request_info_data.cc shared_impl/url_response_info_data.cc shared_impl/var.cc shared_impl/var_tracker.cc shared_impl/vpn_provider_util.cc shared_impl/private/net_address_private_impl.cc shared_impl/private/net_address_private_impl_constants.cc shared_impl/private/ppb_char_set_shared.cc shared_impl/private/ppb_x509_certificate_private_shared.cc)


# all files refer to "ui/...", so we include "../.." here
# we include GPU_SRC_DIR for "GLES2/gl2chromium.h"
SET(include_dirs ${INCLUDE_DIRS} ${GPU_SRC_DIR} ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${LINK_LIBRARIES})

ADD_LIBRARY(ppapi SHARED $<TARGET_OBJECTS:cpp> $<TARGET_OBJECTS:thunk> $<TARGET_OBJECTS:sharedimpl>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ppapi ${link_libraries})

INSTALL(TARGETS ppapi
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
