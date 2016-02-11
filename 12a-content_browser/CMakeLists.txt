PROJECT(chromium-content-browser CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Content Browser")

# Location of direct dependencies
SET(TRACING_DIR "${CMAKE_SOURCE_DIR}/../components/tracing")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/..")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES common/font_list_ozone.cc common/cursors/webcursor_ozone.cc common/gpu/gpu_memory_buffer_factory_ozone.cc common/gpu/gpu_memory_buffer_factory_ozone_native_pixmap.cc common/gpu/client/gpu_memory_buffer_impl_ozone.cc common/gpu/client/gpu_memory_buffer_impl_ozone_native_pixmap.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xcomposite)
  SET(BACKEND_SOURCE_FILES common/cursors/webcursor_aurax11.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DCONTENT_IMPLEMENTATION -DTRACING_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .GRD -> .CC,.H,.PAK

MESSAGE(STATUS "Generating .cc,.h,.pak from .grd...")

EXECUTE_PROCESS(COMMAND mkdir -p content/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i content_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/content --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# tracing
ADD_LIBRARY(tracing OBJECT ${TRACING_DIR}/child_memory_dump_manager_delegate_impl.cc ${TRACING_DIR}/child_trace_message_filter.cc ${TRACING_DIR}/trace_config_file.cc ${TRACING_DIR}/trace_to_console.cc ${TRACING_DIR}/tracing_messages.cc ${TRACING_DIR}/tracing_switches.cc)

# common
ADD_LIBRARY(common OBJECT public/common/child_process_host_delegate.cc public/common/color_suggestion.cc public/common/common_param_traits.cc public/common/content_client.cc public/common/content_constants.cc public/common/content_features.cc public/common/content_switches.cc public/common/context_menu_params.cc public/common/drop_data.cc public/common/favicon_url.cc public/common/file_chooser_file_info.cc public/common/file_chooser_params.cc public/common/frame_navigate_params.cc public/common/geoposition.cc public/common/manifest.cc public/common/menu_item.cc public/common/message_port_types.cc public/common/navigator_connect_client.cc public/common/page_state.cc public/common/platform_notification_data.cc public/common/presentation_constants.cc public/common/push_messaging_status.cc public/common/referrer.cc public/common/renderer_preferences.cc public/common/resource_devtools_info.cc public/common/resource_response.cc public/common/resource_response_info.cc public/common/resource_type.cc public/common/sandboxed_process_launcher_delegate.cc public/common/signed_certificate_timestamp_id_and_status.cc public/common/speech_recognition_result.cc public/common/ssl_status.cc public/common/url_constants.cc public/common/url_utils.cc public/common/web_preferences.cc public/common/webplugininfo.cc public/common/window_container_type.cc common/appcache_interfaces.cc common/ax_content_node_data.cc common/cc_messages.cc common/child_process_host_impl.cc common/child_process_sandbox_support_impl_linux.cc common/child_process_sandbox_support_impl_shm_linux.cc common/content_constants_internal.cc common/content_ipc_logging.cc common/content_message_generator.cc common/content_param_traits.cc common/content_paths.cc common/content_switches_internal.cc common/cross_site_document_classifier.cc common/discardable_shared_memory_heap.cc common/font_config_ipc_linux.cc common/font_list.cc common/frame_param.cc common/frame_replication_state.cc common/gamepad_param_traits.cc common/gamepad_user_gesture.cc common/generic_shared_memory_id_generator.cc common/geofencing_types.cc common/host_discardable_shared_memory_manager.cc common/host_shared_bitmap_manager.cc common/in_process_child_thread_params.cc common/inter_process_time_ticks_converter.cc common/message_router.cc common/navigation_params.cc common/one_writer_seqlock.cc common/origin_util.cc common/page_state_serialization.cc common/page_zoom.cc common/process_type.cc common/resource_messages.cc common/resource_request_body.cc common/savable_url_schemes.cc common/send_zygote_child_ping_linux.cc common/service_port_type_converters.cc common/set_process_title.cc common/set_process_title_linux.cc common/site_isolation_policy.cc common/ssl_status_serialization.cc common/swapped_out_messages.cc common/url_schemes.cc common/user_agent.cc common/webplugin_geometry.cc common/websocket.cc)

# bluetooth
ADD_LIBRARY(bluetooth OBJECT common/bluetooth/bluetooth_device.cc common/bluetooth/bluetooth_scan_filter.cc)

# browser_plugin
ADD_LIBRARY(browser_plugin OBJECT public/common/browser_plugin_guest_mode.cc common/browser_plugin/browser_plugin_constants.cc)

# cache_storage
ADD_LIBRARY(cache_storage OBJECT common/cache_storage/cache_storage_types.cc)

# dom_storage
ADD_LIBRARY(dom_storage OBJECT common/dom_storage/dom_storage_map.cc)

# cursors
ADD_LIBRARY(cursors OBJECT common/cursors/webcursor.cc common/cursors/webcursor_aura.cc)

# gpu
ADD_LIBRARY(gpu OBJECT public/common/gpu_memory_stats.cc common/gpu/gpu_channel.cc common/gpu/gpu_channel_manager.cc common/gpu/gpu_command_buffer_stub.cc common/gpu/gpu_memory_buffer_factory.cc common/gpu/gpu_memory_manager.cc common/gpu/gpu_memory_tracking.cc common/gpu/gpu_surface_lookup.cc common/gpu/image_transport_surface.cc common/gpu/image_transport_surface_linux.cc) 

# gpu_client
ADD_LIBRARY(gpu_client OBJECT common/gpu/client/command_buffer_metrics.cc common/gpu/client/command_buffer_proxy_impl.cc common/gpu/client/context_provider_command_buffer.cc common/gpu/client/gl_helper.cc common/gpu/client/gl_helper_readback_support.cc common/gpu/client/gl_helper_scaling.cc common/gpu/client/gpu_channel_host.cc common/gpu/client/gpu_jpeg_decode_accelerator_host.cc common/gpu/client/gpu_memory_buffer_impl.cc common/gpu/client/gpu_memory_buffer_impl_shared_memory.cc common/gpu/client/gpu_video_decode_accelerator_host.cc common/gpu/client/gpu_video_encode_accelerator_host.cc common/gpu/client/grcontext_for_webgraphicscontext3d.cc common/gpu/client/webgraphicscontext3d_command_buffer_impl.cc)

# gpu_media
ADD_LIBRARY(gpu_media OBJECT common/gpu/media/fake_video_decode_accelerator.cc common/gpu/media/gpu_jpeg_decode_accelerator.cc common/gpu/media/gpu_video_accelerator_util.cc common/gpu/media/gpu_video_decode_accelerator.cc common/gpu/media/gpu_video_encode_accelerator.cc)

# indexed_db
ADD_LIBRARY(indexed_db OBJECT common/indexed_db/indexed_db_key.cc common/indexed_db/indexed_db_key_path.cc common/indexed_db/indexed_db_key_range.cc common/indexed_db/indexed_db_param_traits.cc)

# input
ADD_LIBRARY(input OBJECT common/input/did_overscroll_params.cc common/input/gesture_event_stream_validator.cc common/input/input_event.cc common/input/input_event_ack.cc common/input/input_event_stream_validator.cc common/input/input_param_traits.cc common/input/scoped_web_input_event.cc common/input/synthetic_gesture_packet.cc common/input/synthetic_gesture_params.cc common/input/synthetic_pinch_gesture_params.cc common/input/synthetic_smooth_drag_gesture_params.cc common/input/synthetic_tap_gesture_params.cc common/input/synthetic_web_input_event_builders.cc common/input/touch_event_stream_validator.cc common/input/web_input_event_traits.cc common/input/web_touch_event_traits.cc)

# media
ADD_LIBRARY(media OBJECT public/common/media_stream_request.cc common/media/media_param_traits.cc common/media/media_stream_options.cc)

# mojo
ADD_LIBRARY(mojo OBJECT public/common/mojo_channel_switches.cc common/mojo/channel_init.cc common/mojo/service_registry_for_route.cc common/mojo/service_registry_impl.cc)

# net
ADD_LIBRARY(net OBJECT common/net/url_fetcher.cc common/net/url_request_user_data.cc)

# pepper
# ADD_LIBRARY(pepper OBJECT public/common/pepper_plugin_info.cc common/pepper_file_util.cc common/pepper_plugin_list.cc common/pepper_render_instance_data.cc common/plugin_list.cc common/plugin_list_posix.cc common/sandbox_util.cc)
# Pepper (PPAPI) disabled here !

# sandbox
# ADD_LIBRARY(sandbox OBJECT common/sandbox_linux/bpf_cros_arm_gpu_policy common/sandbox_linux/bpf_gpu_policy_linux.cc common/sandbox_linux/bpf_ppapi_policy_linux.cc common/sandbox_linux/bpf_renderer_policy_linux.cc common/sandbox_linux/bpf_utility_policy_linux.cc common/sandbox_linux/sandbox_bpf_base_policy_linux.cc common/sandbox_linux/sandbox_debug_handling_linux.cc common/sandbox_linux/sandbox_seccomp_bpf_linux.cc common/sandbox_linux/sandbox_init_linux.cc common/sandbox_linux/sandbox_linux.cc)
# ADD_DEFINITIONS(-DUSE_SECCOMP_BPF)
# Sandboxing disabled here !

# service_worker
ADD_LIBRARY(service_worker OBJECT common/service_worker/service_worker_client_info.cc common/service_worker/service_worker_status_code.cc common/service_worker/service_worker_types.cc common/service_worker/service_worker_type_converters.cc common/service_worker/service_worker_utils.cc)


# all files refer to "content/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${backend_LIBRARIES})

ADD_LIBRARY(content_browser SHARED $<TARGET_OBJECTS:tracing> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:bluetooth> $<TARGET_OBJECTS:browser_plugin> $<TARGET_OBJECTS:cache_storage> $<TARGET_OBJECTS:dom_storage> $<TARGET_OBJECTS:cursors> $<TARGET_OBJECTS:gpu> $<TARGET_OBJECTS:gpu_client> $<TARGET_OBJECTS:gpu_media> $<TARGET_OBJECTS:indexed_db> $<TARGET_OBJECTS:input> $<TARGET_OBJECTS:media> $<TARGET_OBJECTS:mojo> $<TARGET_OBJECTS:net> $<TARGET_OBJECTS:service_worker> ${BACKEND_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(content_browser ${link_libraries})

INSTALL(TARGETS content_browser
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
