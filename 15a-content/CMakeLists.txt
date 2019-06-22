PROJECT(chromium-content CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Content")

# Location of direct dependencies
SET(STARTUP_METRIC_DIR "${CMAKE_SOURCE_DIR}/../components/startup_metric_utils")
SET(URL_FORMATTER_DIR "${CMAKE_SOURCE_DIR}/../components/url_formatter")
SET(SCHEDULER_DIR "${CMAKE_SOURCE_DIR}/../components/scheduler")
SET(MIME_UTIL_DIR "${CMAKE_SOURCE_DIR}/../components/mime_util")
SET(TRACING_DIR "${CMAKE_SOURCE_DIR}/../components/tracing")
SET(TRACING_EXT_DIR "${CMAKE_SOURCE_DIR}/../third_party/catapult/tracing")
SET(WEBCRYPTO_DIR "${CMAKE_SOURCE_DIR}/../components/webcrypto")
SET(SANDBOX_DIR "${CMAKE_SOURCE_DIR}/../sandbox")
SET(GOOGLE_APIS_DIR "${CMAKE_SOURCE_DIR}/../google_apis")
SET(UI_STRINGS_DIR "${CMAKE_SOURCE_DIR}/../ui/strings")
SET(UI_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/../ui/resources")
SET(UI_GFX_DIR "${CMAKE_SOURCE_DIR}/../ui/gfx")
SET(UI_ACCESSIBILITY_DIR "${CMAKE_SOURCE_DIR}/../ui/accessibility")
SET(MOJO_SRC_DIR "${CMAKE_SOURCE_DIR}/../mojo")
SET(ZLIB_EXT_DIR "${CMAKE_SOURCE_DIR}/../third_party/zlib")
SET(LEVELDB_EXT_DIR "${CMAKE_SOURCE_DIR}/../third_party/leveldatabase")
SET(ANGLE_SRC_DIR "${CMAKE_SOURCE_DIR}/../third_party/angle")
SET(WIDEVINE_DIR "${CMAKE_SOURCE_DIR}/../third_party/widevine")
SET(WEBRTC_DIR "${CMAKE_SOURCE_DIR}/../third_party/webrtc")
SET(NPAPI_DIR "${CMAKE_SOURCE_DIR}/../third_party/npapi")
SET(BLINK_DIR "${CMAKE_SOURCE_DIR}/../third_party/WebKit")
SET(V8_DIR "${CMAKE_SOURCE_DIR}/../v8")
SET(CC_DIR "${CMAKE_SOURCE_DIR}/../cc")

# Location of Skia source folders
SET(SKIA_SRC_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../gpu")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/..")
SET(BUILD_DIR "${CMAKE_SOURCE_DIR}/..")
SET(PROTOC_WRAPPER_DIR "${CMAKE_SOURCE_DIR}/..")

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
PKG_CHECK_MODULES(deps REQUIRED leveldb gconf-2.0 atk)
# TODO : pull these dependencies with .pc files !
PKG_CHECK_MODULES(nss REQUIRED nss)
PKG_CHECK_MODULES(minizip REQUIRED minizip)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

FIND_PROGRAM(PROTOC_FOUND protoc)
IF(NOT PROTOC_FOUND)
  MESSAGE(FATAL_ERROR "\"protoc\" is required, please install the \"protobuf-compiler\" package !")
ELSE()
  MESSAGE(STATUS "\"protoc\" found...")
ENDIF(NOT PROTOC_FOUND)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES common/font_list_ozone.cc common/cursors/webcursor_ozone.cc common/gpu/gpu_memory_buffer_factory_ozone_native_pixmap.cc common/gpu/client/gpu_memory_buffer_impl_ozone_native_pixmap.cc browser/power_save_blocker_ozone.cc browser/accessibility/accessibility_tree_formatter_stub.cc browser/compositor/browser_compositor_overlay_candidate_validator_ozone.cc browser/compositor/software_output_device_ozone.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xcomposite pangocairo)
  SET(BACKEND_SOURCE_FILES common/font_list_pango.cc common/cursors/webcursor_aurax11.cc browser/power_save_blocker_ozone.cc browser/accessibility/accessibility_tree_formatter_auralinux.cc browser/accessibility/browser_accessibility_auralinux.cc browser/accessibility/browser_accessibility_manager_auralinux.cc browser/compositor/software_output_device_x11.cc)
# "power_save_blocker_x11.cc" pulls D-Bus; to avoid it, we replace it with
# "power_save_blocker_ozone.cc", which is more and less a stub implementation
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DCONTENT_IMPLEMENTATION -DTRACING_IMPLEMENTATION -DSCHEDULER_IMPLEMENTATION -DSANDBOX_IMPLEMENTATION -DUSE_DEFAULT_RENDER_THEME -DUSE_SYSTEM_MINIZIP -DNO_TCMALLOC -DOS_LINUX -DOS_POSIX -DUSE_AURA -D_FILE_OFFSET_BITS=64)
# This is required by some versions of Mesa
ADD_DEFINITIONS(-DMESA_EGL_NO_X11_HEADERS)
# This is because we are including "inttypes.h" with a Mojo header
ADD_DEFINITIONS(-D__STDC_FORMAT_MACROS)
# Required for our BoringSSL->NSS patches
ADD_DEFINITIONS(-DUSE_NSS_CERTS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .HTML,.JS

MESSAGE(STATUS "Generating .html,.js...")

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/tracing WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${TRACING_EXT_DIR}/bin/generate_about_tracing_contents --outdir ${CMAKE_BINARY_DIR}/content/browser/tracing WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .HTML,.JS -> .GRD

MESSAGE(STATUS "Generating .grd from .html,.js...")

EXECUTE_PROCESS(COMMAND python browser/tracing/generate_trace_viewer_grd.py ${CMAKE_BINARY_DIR}/content/browser/tracing/about_tracing.html ${CMAKE_BINARY_DIR}/content/browser/tracing/about_tracing.js --output ${CMAKE_BINARY_DIR}/content/browser/tracing/tracing_resources.grd WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .GRD -> .CC,.H,.PAK

MESSAGE(STATUS "Generating .cc,.h,.pak from .grd...")

EXECUTE_PROCESS(COMMAND mkdir -p content/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p blink/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i content_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/content --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i app/resources/content_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/content/app/resources --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i app/strings/content_strings.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/content/app/strings --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ${BLINK_DIR}/Source/Build/devtools/devtools_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/blink --write-only-new=0 -D SHARED_INTERMEDIATE_DIR=${BLINK_DIR}/Source/Build WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# TODO : "Source/Build" is a HACK !
EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ${CMAKE_BINARY_DIR}/content/browser/tracing/tracing_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/content/browser/tracing --write-only-new=0 -D SHARED_INTERMEDIATE_DIR=${CMAKE_BINARY_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/routed_service_provider.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/permission_service.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/application_setup.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/presentation/presentation_service.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/image_downloader/image_downloader.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/service_worker/embedded_worker_setup.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py public/common/service_worker_event_status.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py public/common/permission_status.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py public/common/mojo_geoposition.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py public/common/background_sync.mojom --use_bundled_pylibs -d . -I. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/background_sync_service.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/geolocation_service.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/wake_lock_service.mojom --use_bundled_pylibs -d . -I.. -o ${CMAKE_BINARY_DIR}/content WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py common/process_control.mojom --use_bundled_pylibs -d .. -I.. -I${MOJO_SRC_DIR}/services -o ${CMAKE_BINARY_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .PROTO -> .CC,.H,.PY

MESSAGE(STATUS "Generating .cc,.h,.py files from .proto...")

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/background_sync/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir browser/background_sync --proto-in-file background_sync.proto --use-system-protobuf=1 -- protoc --cpp_out ${CMAKE_BINARY_DIR}/content/browser/background_sync WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/cache_storage/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir browser/cache_storage --proto-in-file cache_storage.proto --use-system-protobuf=1 -- protoc --cpp_out ${CMAKE_BINARY_DIR}/content/browser/cache_storage WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/notifications/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir browser/notifications --proto-in-file notification_database_data.proto --use-system-protobuf=1 -- protoc --cpp_out ${CMAKE_BINARY_DIR}/content/browser/notifications WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/service_worker/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir browser/service_worker --proto-in-file service_worker_database.proto --use-system-protobuf=1 -- protoc --cpp_out ${CMAKE_BINARY_DIR}/content/browser/service_worker WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/speech/proto/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir browser/speech/proto --proto-in-file google_streaming_api.proto --use-system-protobuf=1 -- protoc --cpp_out ${CMAKE_BINARY_DIR}/content/browser/speech/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .CC,.H FROM .JSON

MESSAGE(STATUS "Generating .cc,.h files from .json...")

EXECUTE_PROCESS(COMMAND mkdir -p content/browser/devtools/protocol/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python browser/devtools/protocol/devtools_protocol_handler_generator.py ${BLINK_DIR}/Source/devtools/protocol.json browser/devtools/browser_protocol.json ${CMAKE_BINARY_DIR}/content/browser/devtools/protocol/devtools_protocol_dispatcher.cc ${CMAKE_BINARY_DIR}/content/browser/devtools/protocol/devtools_protocol_dispatcher.h WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# PRE-GENERATION WEBKIT_VERSION.H

MESSAGE(STATUS "Generating webkit_vesion.h from webkit_version.h.in...")

EXECUTE_PROCESS(COMMAND mkdir -p build/util/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND sh -c "python ${BUILD_DIR}/build/util/lastchange.py > ${CMAKE_BINARY_DIR}/LASTCHANGE.blink" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${BUILD_DIR}/build/util/version.py -f ${CMAKE_BINARY_DIR}/LASTCHANGE.blink ${BUILD_DIR}/build/util/webkit_version.h.in ${CMAKE_BINARY_DIR}/build/util/webkit_version.h WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# startup_metrics
ADD_LIBRARY(startup_metrics OBJECT ${STARTUP_METRIC_DIR}/common/startup_metric_message_generator.cc ${STARTUP_METRIC_DIR}/browser/startup_metric_message_filter.cc ${STARTUP_METRIC_DIR}/browser/startup_metric_utils.cc)

# url_formatter
ADD_LIBRARY(url_formatter OBJECT ${URL_FORMATTER_DIR}/elide_url.cc ${URL_FORMATTER_DIR}/url_fixer.cc ${URL_FORMATTER_DIR}/url_formatter.cc)

# scheduler
ADD_LIBRARY(scheduler OBJECT ${SCHEDULER_DIR}/common/scheduler_switches.cc ${SCHEDULER_DIR}/base/cancelable_closure_holder.cc ${SCHEDULER_DIR}/base/lazy_now.cc ${SCHEDULER_DIR}/base/real_time_domain.cc ${SCHEDULER_DIR}/base/task_queue.cc ${SCHEDULER_DIR}/base/task_queue_impl.cc ${SCHEDULER_DIR}/base/task_queue_manager.cc ${SCHEDULER_DIR}/base/task_queue_selector.cc ${SCHEDULER_DIR}/base/time_domain.cc ${SCHEDULER_DIR}/base/work_queue.cc ${SCHEDULER_DIR}/base/work_queue_sets.cc ${SCHEDULER_DIR}/base/pollable_thread_safe_flag.cc ${SCHEDULER_DIR}/base/virtual_time_domain.cc ${SCHEDULER_DIR}/base/enqueue_order.cc ${SCHEDULER_DIR}/child/idle_helper.cc ${SCHEDULER_DIR}/child/scheduler_helper.cc ${SCHEDULER_DIR}/child/scheduler_tqm_delegate_impl.cc ${SCHEDULER_DIR}/child/single_thread_idle_task_runner.cc ${SCHEDULER_DIR}/child/virtual_time_tqm_delegate.cc ${SCHEDULER_DIR}/child/web_scheduler_impl.cc ${SCHEDULER_DIR}/child/web_task_runner_impl.cc ${SCHEDULER_DIR}/child/webthread_base.cc ${SCHEDULER_DIR}/child/webthread_impl_for_worker_scheduler.cc ${SCHEDULER_DIR}/child/worker_scheduler.cc ${SCHEDULER_DIR}/child/worker_scheduler_impl.cc ${SCHEDULER_DIR}/renderer/deadline_task_runner.cc ${SCHEDULER_DIR}/renderer/idle_time_estimator.cc ${SCHEDULER_DIR}/renderer/renderer_scheduler.cc ${SCHEDULER_DIR}/renderer/renderer_scheduler_impl.cc ${SCHEDULER_DIR}/renderer/renderer_web_scheduler_impl.cc ${SCHEDULER_DIR}/renderer/render_widget_scheduling_state.cc ${SCHEDULER_DIR}/renderer/render_widget_signals.cc ${SCHEDULER_DIR}/renderer/task_cost_estimator.cc ${SCHEDULER_DIR}/renderer/throttling_helper.cc ${SCHEDULER_DIR}/renderer/web_frame_scheduler_impl.cc ${SCHEDULER_DIR}/renderer/web_view_scheduler_impl.cc ${SCHEDULER_DIR}/renderer/user_model.cc ${SCHEDULER_DIR}/renderer/webthread_impl_for_renderer_scheduler.cc)

# tracing
ADD_LIBRARY(tracing OBJECT ${TRACING_DIR}/child_memory_dump_manager_delegate_impl.cc ${TRACING_DIR}/child_trace_message_filter.cc ${TRACING_DIR}/trace_config_file.cc ${TRACING_DIR}/trace_to_console.cc ${TRACING_DIR}/tracing_messages.cc ${TRACING_DIR}/tracing_switches.cc)

# webcrypto
ADD_LIBRARY(webcrypto OBJECT ${WEBCRYPTO_DIR}/webcrypto_impl.cc ${WEBCRYPTO_DIR}/webcrypto_util.cc ${WEBCRYPTO_DIR}/algorithm_dispatch.cc ${WEBCRYPTO_DIR}/algorithm_implementation.cc ${WEBCRYPTO_DIR}/algorithm_registry.cc ${WEBCRYPTO_DIR}/crypto_data.cc ${WEBCRYPTO_DIR}/generate_key_result.cc ${WEBCRYPTO_DIR}/jwk.cc ${WEBCRYPTO_DIR}/status.cc ${WEBCRYPTO_DIR}/nss/aes_algorithm_nss.cc ${WEBCRYPTO_DIR}/nss/aes_cbc_nss.cc ${WEBCRYPTO_DIR}/nss/aes_gcm_nss.cc ${WEBCRYPTO_DIR}/nss/aes_kw_nss.cc ${WEBCRYPTO_DIR}/nss/hmac_nss.cc ${WEBCRYPTO_DIR}/nss/key_nss.cc ${WEBCRYPTO_DIR}/nss/rsa_hashed_algorithm_nss.cc ${WEBCRYPTO_DIR}/nss/rsa_oaep_nss.cc ${WEBCRYPTO_DIR}/nss/rsa_ssa_nss.cc ${WEBCRYPTO_DIR}/nss/sha_nss.cc ${WEBCRYPTO_DIR}/nss/sym_key_nss.cc ${WEBCRYPTO_DIR}/nss/util_nss.cc)

# sandbox
ADD_LIBRARY(sandbox OBJECT ${SANDBOX_DIR}/linux/services/init_process_reaper.cc ${SANDBOX_DIR}/linux/services/credentials.cc ${SANDBOX_DIR}/linux/services/namespace_sandbox.cc ${SANDBOX_DIR}/linux/services/namespace_utils.cc ${SANDBOX_DIR}/linux/services/proc_util.cc ${SANDBOX_DIR}/linux/services/resource_limits.cc ${SANDBOX_DIR}/linux/services/scoped_process.cc ${SANDBOX_DIR}/linux/services/syscall_wrappers.cc ${SANDBOX_DIR}/linux/services/thread_helpers.cc ${SANDBOX_DIR}/linux/services/yama.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_channel.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_client.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_file_permission.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_host.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_policy.cc ${SANDBOX_DIR}/linux/syscall_broker/broker_process.cc ${SANDBOX_DIR}/linux/suid/client/setuid_sandbox_client.cc ${SANDBOX_DIR}/linux/suid/client/setuid_sandbox_host.cc)

# google_apis
ADD_LIBRARY(google_apis OBJECT ${GOOGLE_APIS_DIR}/gaia/account_tracker.cc ${GOOGLE_APIS_DIR}/gaia/gaia_auth_consumer.cc ${GOOGLE_APIS_DIR}/gaia/gaia_auth_fetcher.cc ${GOOGLE_APIS_DIR}/gaia/gaia_auth_util.cc ${GOOGLE_APIS_DIR}/gaia/gaia_constants.cc ${GOOGLE_APIS_DIR}/gaia/gaia_oauth_client.cc ${GOOGLE_APIS_DIR}/gaia/gaia_switches.cc ${GOOGLE_APIS_DIR}/gaia/gaia_urls.cc ${GOOGLE_APIS_DIR}/gaia/google_service_auth_error.cc ${GOOGLE_APIS_DIR}/gaia/identity_provider.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_access_token_fetcher.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_access_token_fetcher_impl.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_access_token_fetcher_immediate_error.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_api_call_flow.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_mint_token_flow.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_token_service.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_token_service_delegate.cc ${GOOGLE_APIS_DIR}/gaia/oauth2_token_service_request.cc ${GOOGLE_APIS_DIR}/gaia/oauth_request_signer.cc ${GOOGLE_APIS_DIR}/gaia/ubertoken_fetcher.cc ${GOOGLE_APIS_DIR}/google_api_keys.cc)

# common
ADD_LIBRARY(common OBJECT public/common/child_process_host_delegate.cc public/common/color_suggestion.cc public/common/common_param_traits.cc public/common/content_client.cc public/common/content_constants.cc public/common/content_features.cc public/common/content_switches.cc public/common/context_menu_params.cc public/common/drop_data.cc public/common/favicon_url.cc public/common/file_chooser_file_info.cc public/common/file_chooser_params.cc public/common/frame_navigate_params.cc public/common/geoposition.cc public/common/manifest.cc public/common/menu_item.cc public/common/message_port_types.cc public/common/page_state.cc public/common/platform_notification_data.cc public/common/presentation_constants.cc public/common/push_messaging_status.cc public/common/referrer.cc public/common/renderer_preferences.cc public/common/resource_devtools_info.cc public/common/resource_response.cc public/common/resource_response_info.cc public/common/resource_type.cc public/common/sandboxed_process_launcher_delegate.cc public/common/signed_certificate_timestamp_id_and_status.cc public/common/speech_recognition_result.cc public/common/ssl_status.cc public/common/url_constants.cc public/common/url_utils.cc public/common/web_preferences.cc public/common/webplugininfo.cc public/common/window_container_type.cc common/appcache_interfaces.cc common/ax_content_node_data.cc common/cc_messages.cc common/child_process_host_impl.cc common/child_process_sandbox_support_impl_linux.cc common/child_process_sandbox_support_impl_shm_linux.cc common/content_constants_internal.cc common/content_ipc_logging.cc common/content_message_generator.cc common/content_param_traits.cc common/content_paths.cc common/content_switches_internal.cc common/cross_site_document_classifier.cc common/discardable_shared_memory_heap.cc common/font_config_ipc_linux.cc common/font_list.cc common/frame_param.cc common/frame_replication_state.cc common/gamepad_param_traits.cc common/gamepad_user_gesture.cc common/generic_shared_memory_id_generator.cc common/host_discardable_shared_memory_manager.cc common/host_shared_bitmap_manager.cc common/in_process_child_thread_params.cc common/inter_process_time_ticks_converter.cc common/message_router.cc common/navigation_params.cc common/one_writer_seqlock.cc common/origin_util.cc common/page_state_serialization.cc common/page_zoom.cc common/process_type.cc common/resource_messages.cc common/resource_request_body.cc common/savable_url_schemes.cc common/send_zygote_child_ping_linux.cc common/set_process_title.cc common/set_process_title_linux.cc common/site_isolation_policy.cc common/ssl_status_serialization.cc common/swapped_out_messages.cc common/url_schemes.cc common/user_agent.cc common/webplugin_geometry.cc common/websocket.cc)

# common_bluetooth
#ADD_LIBRARY(common_bluetooth OBJECT common/bluetooth/bluetooth_device.cc common/bluetooth/bluetooth_scan_filter.cc)
# Bluetooth DISABLED !

# common_cache_storage
ADD_LIBRARY(common_cache_storage OBJECT common/cache_storage/cache_storage_types.cc)

# common_cursors
ADD_LIBRARY(common_cursors OBJECT common/cursors/webcursor.cc common/cursors/webcursor_aura.cc)

# common_dom_storage
ADD_LIBRARY(common_dom_storage OBJECT common/dom_storage/dom_storage_map.cc)

# common_gpu
ADD_LIBRARY(common_gpu OBJECT public/common/gpu_memory_stats.cc common/gpu/gpu_channel.cc common/gpu/gpu_channel_manager.cc common/gpu/gpu_command_buffer_stub.cc common/gpu/gpu_memory_buffer_factory.cc common/gpu/gpu_memory_manager.cc common/gpu/gpu_memory_tracking.cc common/gpu/gpu_surface_lookup.cc common/gpu/image_transport_surface.cc common/gpu/image_transport_surface_linux.cc) 

# common_gpu_client
ADD_LIBRARY(common_gpu_client OBJECT common/gpu/client/command_buffer_metrics.cc common/gpu/client/command_buffer_proxy_impl.cc common/gpu/client/context_provider_command_buffer.cc common/gpu/client/gl_helper.cc common/gpu/client/gl_helper_readback_support.cc common/gpu/client/gl_helper_scaling.cc common/gpu/client/gpu_channel_host.cc common/gpu/client/gpu_jpeg_decode_accelerator_host.cc common/gpu/client/gpu_memory_buffer_impl.cc common/gpu/client/gpu_memory_buffer_impl_shared_memory.cc common/gpu/client/gpu_video_decode_accelerator_host.cc common/gpu/client/gpu_video_encode_accelerator_host.cc common/gpu/client/grcontext_for_webgraphicscontext3d.cc common/gpu/client/webgraphicscontext3d_command_buffer_impl.cc)

# common_gpu_media
ADD_LIBRARY(common_gpu_media OBJECT common/gpu/media/fake_video_decode_accelerator.cc common/gpu/media/gpu_jpeg_decode_accelerator.cc common/gpu/media/gpu_video_accelerator_util.cc common/gpu/media/gpu_video_decode_accelerator.cc common/gpu/media/gpu_video_encode_accelerator.cc)

# common_indexeddb
ADD_LIBRARY(common_indexeddb OBJECT common/indexed_db/indexed_db_key.cc common/indexed_db/indexed_db_key_path.cc common/indexed_db/indexed_db_key_range.cc common/indexed_db/indexed_db_param_traits.cc)

# common_input
ADD_LIBRARY(common_input OBJECT common/input/did_overscroll_params.cc common/input/gesture_event_stream_validator.cc common/input/input_event.cc common/input/input_event_ack.cc common/input/input_event_stream_validator.cc common/input/input_param_traits.cc common/input/scoped_web_input_event.cc common/input/synthetic_gesture_packet.cc common/input/synthetic_gesture_params.cc common/input/synthetic_pinch_gesture_params.cc common/input/synthetic_smooth_drag_gesture_params.cc common/input/synthetic_smooth_scroll_gesture_params.cc common/input/synthetic_tap_gesture_params.cc common/input/synthetic_web_input_event_builders.cc common/input/touch_event_stream_validator.cc common/input/web_input_event_traits.cc common/input/web_touch_event_traits.cc)

# common_media
ADD_LIBRARY(common_media OBJECT public/common/media_stream_request.cc common/media/media_param_traits.cc common/media/media_stream_options.cc)

# common_mojo
ADD_LIBRARY(common_mojo OBJECT public/common/mojo_channel_switches.cc common/mojo/channel_init.cc common/mojo/service_registry_for_route.cc common/mojo/service_registry_impl.cc)

# common_net
ADD_LIBRARY(common_net OBJECT common/net/url_fetcher.cc common/net/url_request_user_data.cc)

# common_plugin
ADD_LIBRARY(common_plugin OBJECT public/common/browser_plugin_guest_mode.cc common/browser_plugin/browser_plugin_constants.cc)

# common_service_worker
ADD_LIBRARY(common_service_worker OBJECT common/service_worker/service_worker_client_info.cc common/service_worker/service_worker_status_code.cc common/service_worker/service_worker_types.cc common/service_worker/service_worker_type_converters.cc common/service_worker/service_worker_utils.cc)

# browser
ADD_LIBRARY(browser OBJECT public/browser/ax_event_notification_details.cc public/browser/background_sync_parameters.cc public/browser/background_tracing_config.cc public/browser/bluetooth_chooser.cc public/browser/browser_child_process_host_delegate.cc public/browser/browser_child_process_host_iterator.cc public/browser/browser_child_process_observer.cc public/browser/browser_main_parts.cc public/browser/browser_message_filter.cc public/browser/browser_plugin_guest_delegate.cc public/browser/browser_plugin_guest_manager.cc public/browser/content_browser_client.cc public/browser/desktop_media_id.cc public/browser/download_manager_delegate.cc public/browser/download_save_info.cc public/browser/download_url_parameters.cc public/browser/favicon_status.cc public/browser/interstitial_page_delegate.cc public/browser/javascript_dialog_manager.cc public/browser/load_from_memory_cache_details.cc public/browser/media_device_id.cc public/browser/navigation_controller.cc public/browser/navigation_details.cc public/browser/navigation_handle.cc public/browser/navigation_throttle.cc public/browser/notification_registrar.cc public/browser/page_navigator.cc public/browser/presentation_session.cc public/browser/presentation_session_message.cc public/browser/push_messaging_service.cc public/browser/resource_dispatcher_host_delegate.cc public/browser/resource_request_details.cc public/browser/screen_orientation_provider.cc public/browser/security_style_explanations.cc public/browser/service_worker_usage_info.cc public/browser/speech_recognition_session_config.cc public/browser/speech_recognition_session_context.cc public/browser/speech_recognition_session_preamble.cc public/browser/stream_info.cc public/browser/tracing_delegate.cc public/browser/url_data_source.cc public/browser/web_contents.cc public/browser/web_contents_delegate.cc public/browser/web_contents_observer.cc public/browser/web_contents_view_delegate.cc public/browser/web_ui_controller.cc browser/bad_message.cc browser/browser_child_process_host_impl.cc browser/browser_context.cc browser/browser_ipc_logging.cc browser/browser_main.cc browser/browser_main_loop.cc browser/browser_main_runner.cc browser/browser_process_sub_thread.cc browser/browser_shutdown_profile_dumper.cc browser/browser_thread_impl.cc browser/browser_url_handler_impl.cc browser/browsing_instance.cc browser/byte_stream.cc browser/cert_store_impl.cc browser/child_process_launcher.cc browser/child_process_security_policy_impl.cc browser/file_descriptor_info_impl.cc browser/font_list_async.cc browser/histogram_controller.cc browser/histogram_internals_request_job.cc browser/histogram_message_filter.cc browser/histogram_synchronizer.cc browser/host_zoom_level_context.cc browser/host_zoom_map_impl.cc browser/manifest/manifest_manager_host.cc browser/message_port_message_filter.cc browser/message_port_provider.cc browser/message_port_service.cc browser/mime_registry_message_filter.cc browser/notification_service_impl.cc browser/power_monitor_message_broadcaster.cc browser/power_save_blocker_impl.cc browser/profiler_controller_impl.cc browser/profiler_message_filter.cc browser/quota_dispatcher_host.cc browser/resolve_proxy_msg_helper.cc browser/resource_context_impl.cc browser/screen_orientation/screen_orientation_dispatcher_host_impl.cc browser/signed_certificate_timestamp_store_impl.cc browser/site_instance_impl.cc browser/storage_partition_impl.cc browser/storage_partition_impl_map.cc browser/time_zone_monitor.cc browser/time_zone_monitor_linux.cc browser/user_metrics.cc browser/utility_process_host_impl.cc browser/startup_task_runner.cc browser/zygote_host/zygote_host_impl_linux.cc zygote/zygote_linux.cc zygote/zygote_main_linux.cc)

# browser_accessibility
ADD_LIBRARY(browser_accessibility OBJECT browser/accessibility/accessibility_mode_helper.cc browser/accessibility/accessibility_tree_formatter.cc browser/accessibility/accessibility_tree_formatter_blink.cc browser/accessibility/accessibility_ui.cc browser/accessibility/ax_tree_id_registry.cc browser/accessibility/browser_accessibility.cc browser/accessibility/browser_accessibility_manager.cc browser/accessibility/browser_accessibility_state_impl.cc browser/accessibility/one_shot_accessibility_tree_search.cc)

# browser_appcache
ADD_LIBRARY(browser_appcache OBJECT browser/appcache/appcache.cc browser/appcache/appcache_backend_impl.cc browser/appcache/appcache_database.cc browser/appcache/appcache_disk_cache.cc browser/appcache/appcache_dispatcher_host.cc browser/appcache/appcache_frontend_proxy.cc browser/appcache/appcache_group.cc browser/appcache/appcache_histograms.cc browser/appcache/appcache_host.cc browser/appcache/appcache_interceptor.cc browser/appcache/appcache_internals_ui.cc browser/appcache/appcache_manifest_parser.cc browser/appcache/appcache_quota_client.cc browser/appcache/appcache_request_handler.cc browser/appcache/appcache_response.cc browser/appcache/appcache_service_impl.cc browser/appcache/appcache_storage.cc browser/appcache/appcache_storage_impl.cc browser/appcache/appcache_update_job.cc browser/appcache/appcache_url_request_job.cc browser/appcache/appcache_working_set.cc browser/appcache/chrome_appcache_service.cc)

# browser_background_sync
ADD_LIBRARY(browser_background_sync OBJECT browser/background_sync/background_sync_context_impl.cc browser/background_sync/background_sync_manager.cc browser/background_sync/background_sync_metrics.cc browser/background_sync/background_sync_network_observer.cc browser/background_sync/background_sync_power_observer.cc browser/background_sync/background_sync_registration.cc browser/background_sync/background_sync_registration_handle.cc browser/background_sync/background_sync_registration_options.cc browser/background_sync/background_sync_service_impl.cc)

# browser_bluetooth
#ADD_LIBRARY(browser_bluetooth OBJECT browser/bluetooth/bluetooth_dispatcher_host.cc browser/bluetooth/bluetooth_metrics.cc browser/bluetooth/first_device_bluetooth_chooser.cc)
# Bluetooth DISABLED !

# browser_plugin
ADD_LIBRARY(browser_plugin OBJECT browser/browser_plugin/browser_plugin_embedder.cc browser/browser_plugin/browser_plugin_guest.cc browser/browser_plugin/browser_plugin_message_filter.cc)

# browser_cache_storage
ADD_LIBRARY(browser_cache_storage OBJECT browser/cache_storage/cache_storage.cc browser/cache_storage/cache_storage_blob_to_disk_cache.cc browser/cache_storage/cache_storage_cache.cc browser/cache_storage/cache_storage_context_impl.cc browser/cache_storage/cache_storage_dispatcher_host.cc browser/cache_storage/cache_storage_manager.cc browser/cache_storage/cache_storage_quota_client.cc browser/cache_storage/cache_storage_scheduler.cc)

# browser_compositor
ADD_LIBRARY(browser_compositor OBJECT browser/compositor/browser_compositor_output_surface.cc browser/compositor/buffer_queue.cc browser/compositor/delegated_frame_host.cc browser/compositor/gpu_browser_compositor_output_surface.cc browser/compositor/gpu_process_transport_factory.cc browser/compositor/gpu_surfaceless_browser_compositor_output_surface.cc browser/compositor/image_transport_factory.cc browser/compositor/offscreen_browser_compositor_output_surface.cc browser/compositor/owned_mailbox.cc browser/compositor/reflector_impl.cc browser/compositor/reflector_texture.cc browser/compositor/resize_lock.cc browser/compositor/software_browser_compositor_output_surface.cc browser/compositor/surface_utils.cc browser/context_factory.cc)

# browser_device_monitor
#ADD_LIBRARY(browser_device_monitor OBJECT browser/device_monitor_udev.cc browser/udev_linux.cc)
# TODO : this requires UDev

# browser_device_sensors
ADD_LIBRARY(browser_device_sensors OBJECT browser/device_sensors/data_fetcher_shared_memory_base.cc browser/device_sensors/data_fetcher_shared_memory_default.cc browser/device_sensors/device_inertial_sensor_service.cc browser/device_sensors/device_light_message_filter.cc browser/device_sensors/device_motion_message_filter.cc browser/device_sensors/device_orientation_absolute_message_filter.cc browser/device_sensors/device_orientation_message_filter.cc)

# browser_devtools
ADD_LIBRARY(browser_devtools OBJECT browser/devtools/browser_devtools_agent_host.cc browser/devtools/devtools_agent_host_impl.cc browser/devtools/devtools_frame_trace_recorder.cc browser/devtools/devtools_frontend_host_impl.cc browser/devtools/devtools_io_context.cc browser/devtools/devtools_manager.cc browser/devtools/devtools_netlog_observer.cc browser/devtools/devtools_protocol_handler.cc browser/devtools/forwarding_agent_host.cc browser/devtools/protocol/color_picker.cc browser/devtools/protocol/devtools_protocol_client.cc browser/devtools/protocol/dom_handler.cc browser/devtools/protocol/emulation_handler.cc browser/devtools/protocol/input_handler.cc browser/devtools/protocol/inspector_handler.cc browser/devtools/protocol/io_handler.cc browser/devtools/protocol/memory_handler.cc browser/devtools/protocol/network_handler.cc browser/devtools/protocol/page_handler.cc browser/devtools/protocol/security_handler.cc browser/devtools/protocol/service_worker_handler.cc browser/devtools/protocol/system_info_handler.cc browser/devtools/protocol/tethering_handler.cc browser/devtools/protocol/tracing_handler.cc browser/devtools/render_frame_devtools_agent_host.cc browser/devtools/service_worker_devtools_agent_host.cc browser/devtools/service_worker_devtools_manager.cc browser/devtools/shared_worker_devtools_agent_host.cc browser/devtools/shared_worker_devtools_manager.cc browser/devtools/worker_devtools_agent_host.cc)

# browser_dom_storage
ADD_LIBRARY(browser_dom_storage OBJECT browser/dom_storage/dom_storage_area.cc browser/dom_storage/dom_storage_context_impl.cc browser/dom_storage/dom_storage_context_wrapper.cc browser/dom_storage/dom_storage_database.cc browser/dom_storage/dom_storage_host.cc browser/dom_storage/dom_storage_message_filter.cc browser/dom_storage/dom_storage_namespace.cc browser/dom_storage/dom_storage_session.cc browser/dom_storage/dom_storage_task_runner.cc browser/dom_storage/local_storage_database_adapter.cc browser/dom_storage/session_storage_database.cc browser/dom_storage/session_storage_database_adapter.cc browser/dom_storage/session_storage_namespace_impl.cc)

# browser_download
ADD_LIBRARY(browser_download OBJECT browser/download/base_file.cc browser/download/base_file_linux.cc browser/download/base_file_posix.cc browser/download/download_create_info.cc browser/download/download_file_factory.cc browser/download/download_file_impl.cc browser/download/download_interrupt_reasons_impl.cc browser/download/download_item_impl.cc browser/download/download_item_impl_delegate.cc browser/download/download_manager_impl.cc browser/download/download_net_log_parameters.cc browser/download/download_request_core.cc browser/download/download_request_handle.cc browser/download/download_resource_handler.cc browser/download/download_stats.cc browser/download/drag_download_file.cc browser/download/drag_download_util.cc browser/download/file_metadata_linux.cc browser/download/mhtml_generation_manager.cc browser/download/rate_estimator.cc browser/download/save_file.cc browser/download/save_file_manager.cc browser/download/save_file_resource_handler.cc browser/download/save_item.cc browser/download/save_package.cc browser/download/save_types.cc browser/download/url_downloader.cc)

# browser_fileapi
ADD_LIBRARY(browser_fileapi OBJECT browser/fileapi/blob_storage_host.cc browser/fileapi/browser_file_system_helper.cc browser/fileapi/chrome_blob_storage_context.cc browser/fileapi/fileapi_message_filter.cc browser/fileapi/upload_file_system_file_element_reader.cc)

# browser_framehost
ADD_LIBRARY(browser_framehost OBJECT browser/frame_host/cross_process_frame_connector.cc browser/frame_host/cross_site_transferring_request.cc browser/frame_host/debug_urls.cc browser/frame_host/frame_mojo_shell.cc browser/frame_host/frame_navigation_entry.cc browser/frame_host/frame_tree.cc browser/frame_host/frame_tree_node.cc browser/frame_host/interstitial_page_impl.cc browser/frame_host/interstitial_page_navigator_impl.cc browser/frame_host/navigation_controller_impl.cc browser/frame_host/navigation_entry_impl.cc browser/frame_host/navigation_entry_screenshot_manager.cc browser/frame_host/navigation_handle_impl.cc browser/frame_host/navigation_request.cc browser/frame_host/navigation_request_info.cc browser/frame_host/navigator.cc browser/frame_host/navigator_delegate.cc browser/frame_host/navigator_impl.cc browser/frame_host/render_frame_host_delegate.cc browser/frame_host/render_frame_host_factory.cc browser/frame_host/render_frame_host_impl.cc browser/frame_host/render_frame_host_manager.cc browser/frame_host/render_frame_message_filter.cc browser/frame_host/render_frame_proxy_host.cc browser/frame_host/render_widget_host_view_child_frame.cc browser/frame_host/render_widget_host_view_guest.cc)

# browser_gamepad
ADD_LIBRARY(browser_gamepad OBJECT browser/gamepad/gamepad_data_fetcher.cc browser/gamepad/gamepad_platform_data_fetcher.cc browser/gamepad/gamepad_provider.cc browser/gamepad/gamepad_service.cc browser/gamepad/gamepad_standard_mappings.cc browser/gamepad/gamepad_standard_mappings_linux.cc)
# TODO : "browser/gamepad/gamepad_platform_data_fetcher_linux.cc" requires UDev

# browser_geolocation
ADD_LIBRARY(browser_geolocation OBJECT browser/geolocation/empty_wifi_data_provider.cc browser/geolocation/geolocation_provider_impl.cc browser/geolocation/geolocation_service_context.cc browser/geolocation/geolocation_service_impl.cc browser/geolocation/location_arbitrator_impl.cc browser/geolocation/location_provider_base.cc browser/geolocation/network_location_provider.cc browser/geolocation/network_location_request.cc browser/geolocation/wifi_data.cc browser/geolocation/wifi_data_provider.cc browser/geolocation/wifi_data_provider_common.cc browser/geolocation/wifi_data_provider_manager.cc)

# browser_gpu
ADD_LIBRARY(browser_gpu OBJECT browser/gpu/browser_gpu_channel_host_factory.cc browser/gpu/browser_gpu_memory_buffer_manager.cc browser/gpu/compositor_util.cc browser/gpu/gpu_data_manager_impl.cc browser/gpu/gpu_data_manager_impl_private.cc browser/gpu/gpu_internals_ui.cc browser/gpu/gpu_process_host.cc browser/gpu/gpu_process_host_ui_shim.cc browser/gpu/gpu_surface_tracker.cc browser/gpu/shader_disk_cache.cc)

# browser_indexeddb
ADD_LIBRARY(browser_indexeddb OBJECT browser/indexed_db/indexed_db_active_blob_registry.cc browser/indexed_db/indexed_db_backing_store.cc browser/indexed_db/indexed_db_blob_info.cc browser/indexed_db/indexed_db_callbacks.cc browser/indexed_db/indexed_db_class_factory.cc browser/indexed_db/indexed_db_connection.cc browser/indexed_db/indexed_db_context_impl.cc browser/indexed_db/indexed_db_cursor.cc browser/indexed_db/indexed_db_database.cc browser/indexed_db/indexed_db_database_callbacks.cc browser/indexed_db/indexed_db_database_error.cc browser/indexed_db/indexed_db_dispatcher_host.cc browser/indexed_db/indexed_db_factory_impl.cc browser/indexed_db/indexed_db_index_writer.cc browser/indexed_db/indexed_db_internals_ui.cc browser/indexed_db/indexed_db_leveldb_coding.cc browser/indexed_db/indexed_db_metadata.cc browser/indexed_db/indexed_db_pending_connection.cc browser/indexed_db/indexed_db_quota_client.cc browser/indexed_db/indexed_db_transaction.cc browser/indexed_db/indexed_db_transaction_coordinator.cc browser/indexed_db/indexed_db_value.cc browser/indexed_db/leveldb/leveldb_database.cc browser/indexed_db/leveldb/leveldb_env.cc browser/indexed_db/leveldb/leveldb_iterator_impl.cc browser/indexed_db/leveldb/leveldb_transaction.cc browser/indexed_db/leveldb/leveldb_write_batch.cc)

# browser_loader
ADD_LIBRARY(browser_loader OBJECT browser/loader/async_resource_handler.cc browser/loader/async_revalidation_driver.cc browser/loader/async_revalidation_manager.cc browser/loader/certificate_resource_handler.cc browser/loader/cross_site_resource_handler.cc browser/loader/detachable_resource_handler.cc browser/loader/layered_resource_handler.cc browser/loader/mime_type_resource_handler.cc browser/loader/navigation_resource_handler.cc browser/loader/navigation_resource_throttle.cc browser/loader/navigation_url_loader.cc browser/loader/navigation_url_loader_impl.cc browser/loader/navigation_url_loader_impl_core.cc browser/loader/power_save_block_resource_throttle.cc browser/loader/redirect_to_file_resource_handler.cc browser/loader/resource_buffer.cc browser/loader/resource_dispatcher_host_impl.cc browser/loader/resource_handler.cc browser/loader/resource_loader.cc browser/loader/resource_message_delegate.cc browser/loader/resource_message_filter.cc browser/loader/resource_request_info_impl.cc browser/loader/resource_scheduler.cc browser/loader/resource_scheduler_filter.cc browser/loader/stream_resource_handler.cc browser/loader/stream_writer.cc browser/loader/sync_resource_handler.cc browser/loader/stream_writer.cc browser/loader/sync_resource_handler.cc browser/loader/temporary_file_stream.cc browser/loader/throttling_resource_handler.cc browser/loader/upload_data_stream_builder.cc)

# browser_media
ADD_LIBRARY(browser_media OBJECT browser/media/capture/audio_mirroring_manager.cc browser/media/capture/cursor_renderer_aura.cc browser/media/capture/web_contents_audio_input_stream.cc browser/media/capture/web_contents_audio_muter.cc browser/media/capture/web_contents_capture_util.cc browser/media/capture/web_contents_tracker.cc browser/media/capture/web_contents_video_capture_device.cc browser/media/capture/window_activity_tracker_aura.cc browser/media/audio_stream_monitor.cc browser/media/media_internals.cc browser/media/media_internals_handler.cc browser/media/media_internals_proxy.cc browser/media/media_internals_ui.cc browser/media/media_web_contents_observer.cc browser/media/midi_host.cc browser/media/webrtc_identity_store.cc browser/media/webrtc_identity_store_backend.cc)

# browser_memory
ADD_LIBRARY(browser_memory OBJECT browser/memory/memory_message_filter.cc browser/memory/memory_pressure_controller.cc)

# browser_mojo
ADD_LIBRARY(browser_mojo OBJECT browser/mojo/mojo_app_connection_impl.cc browser/mojo/mojo_application_host.cc browser/mojo/mojo_shell_context.cc)

# browser_net
ADD_LIBRARY(browser_net OBJECT browser/net/browser_online_state_observer.cc browser/net/network_errors_listing_ui.cc browser/net/quota_policy_cookie_store.cc browser/net/view_blob_internals_job_factory.cc browser/net/view_http_cache_job_factory.cc)

# browser_notifications
ADD_LIBRARY(browser_notifications OBJECT browser/notifications/notification_database.cc browser/notifications/notification_database_data_conversions.cc browser/notifications/notification_event_dispatcher_impl.cc browser/notifications/notification_id_generator.cc browser/notifications/notification_message_filter.cc browser/notifications/page_notification_delegate.cc browser/notifications/platform_notification_context_impl.cc)

# browser_permissions
ADD_LIBRARY(browser_permissions OBJECT browser/permissions/permission_service_context.cc browser/permissions/permission_service_impl.cc)

# browser_presentation
ADD_LIBRARY(browser_presentation OBJECT browser/presentation/presentation_service_impl.cc browser/presentation/presentation_type_converters.cc)

# browser_push_messaging
ADD_LIBRARY(browser_push_messaging OBJECT browser/push_messaging/push_messaging_message_filter.cc browser/push_messaging/push_messaging_router.cc)

# browser_renderer
ADD_LIBRARY(browser_renderer OBJECT browser/renderer_host/begin_frame_observer_proxy.cc browser/renderer_host/clipboard_message_filter.cc browser/renderer_host/compositor_resize_lock_aura.cc browser/renderer_host/database_message_filter.cc browser/renderer_host/delegated_frame_evictor.cc browser/renderer_host/dip_util.cc browser/renderer_host/file_utilities_message_filter.cc browser/renderer_host/font_utils_linux.cc browser/renderer_host/frame_metadata_util.cc browser/renderer_host/gamepad_browser_message_filter.cc browser/renderer_host/gpu_message_filter.cc browser/renderer_host/native_web_keyboard_event_aura.cc browser/renderer_host/overscroll_configuration.cc browser/renderer_host/overscroll_controller.cc browser/renderer_host/render_message_filter.cc browser/renderer_host/render_process_host_impl.cc browser/renderer_host/render_sandbox_host_linux.cc browser/renderer_host/render_view_host_delegate.cc browser/renderer_host/render_view_host_factory.cc browser/renderer_host/render_view_host_impl.cc browser/renderer_host/render_widget_helper.cc browser/renderer_host/render_widget_host_delegate.cc browser/renderer_host/render_widget_host_impl.cc browser/renderer_host/render_widget_helper.cc browser/renderer_host/render_widget_host_delegate.cc browser/renderer_host/render_widget_host_impl.cc browser/renderer_host/render_widget_host_input_event_router.cc browser/renderer_host/render_widget_host_view_aura.cc browser/renderer_host/render_widget_host_view_base.cc browser/renderer_host/renderer_frame_manager.cc browser/renderer_host/sandbox_ipc_linux.cc browser/renderer_host/ui_events_helper.cc browser/renderer_host/web_input_event_aura.cc browser/renderer_host/websocket_dispatcher_host.cc browser/renderer_host/websocket_host.cc browser/renderer_host/input/gesture_event_queue.cc browser/renderer_host/input/input_router_config_helper.cc browser/renderer_host/input/input_router_impl.cc browser/renderer_host/input/motion_event_web.cc browser/renderer_host/input/render_widget_host_latency_tracker.cc browser/renderer_host/input/stylus_text_selector.cc browser/renderer_host/input/synthetic_gesture.cc browser/renderer_host/input/synthetic_gesture_controller.cc browser/renderer_host/input/synthetic_gesture_target_aura.cc browser/renderer_host/input/synthetic_gesture_target_base.cc browser/renderer_host/input/synthetic_mouse_pointer.cc browser/renderer_host/input/synthetic_pinch_gesture.cc browser/renderer_host/input/synthetic_pointer.cc browser/renderer_host/input/synthetic_pointer_action.cc browser/renderer_host/input/synthetic_smooth_drag_gesture.cc browser/renderer_host/input/synthetic_smooth_move_gesture.cc browser/renderer_host/input/synthetic_smooth_scroll_gesture.cc browser/renderer_host/input/synthetic_tap_gesture.cc browser/renderer_host/input/synthetic_touchpad_pinch_gesture.cc browser/renderer_host/input/synthetic_touchscreen_pinch_gesture.cc browser/renderer_host/input/synthetic_touch_pointer.cc browser/renderer_host/input/tap_suppression_controller.cc browser/renderer_host/input/timeout_monitor.cc browser/renderer_host/input/touch_action_filter.cc browser/renderer_host/input/touch_emulator.cc browser/renderer_host/input/touch_event_queue.cc browser/renderer_host/input/touch_selection_controller_client_aura.cc browser/renderer_host/input/touchpad_tap_suppression_controller.cc browser/renderer_host/input/touchscreen_tap_suppression_controller.cc browser/renderer_host/input/web_input_event_util.cc browser/renderer_host/media/audio_input_debug_writer.cc browser/renderer_host/media/audio_input_device_manager.cc browser/renderer_host/media/audio_input_renderer_host.cc browser/renderer_host/media/audio_input_sync_writer.cc browser/renderer_host/media/audio_output_device_enumerator.cc browser/renderer_host/media/audio_renderer_host.cc browser/renderer_host/media/audio_sync_reader.cc browser/renderer_host/media/media_capture_devices_impl.cc browser/renderer_host/media/media_stream_dispatcher_host.cc browser/renderer_host/media/media_stream_manager.cc browser/renderer_host/media/media_stream_track_metrics_host.cc browser/renderer_host/media/media_stream_ui_proxy.cc browser/renderer_host/media/video_capture_buffer_pool.cc browser/renderer_host/media/video_capture_controller.cc browser/renderer_host/media/video_capture_device_client.cc browser/renderer_host/media/video_capture_gpu_jpeg_decoder.cc browser/renderer_host/media/video_capture_host.cc browser/renderer_host/media/video_capture_manager.cc)
# we remove "memory_benchmark_message_filter.cc"

# browser_service_worker
ADD_LIBRARY(browser_service_worker OBJECT browser/service_worker/embedded_worker_instance.cc browser/service_worker/embedded_worker_registry.cc browser/service_worker/foreign_fetch_request_handler.cc browser/service_worker/service_worker_cache_writer.cc browser/service_worker/service_worker_context_core.cc browser/service_worker/service_worker_context_request_handler.cc browser/service_worker/service_worker_context_watcher.cc browser/service_worker/service_worker_context_wrapper.cc browser/service_worker/service_worker_controllee_request_handler.cc browser/service_worker/service_worker_database.cc browser/service_worker/service_worker_database_task_manager.cc browser/service_worker/service_worker_disk_cache.cc browser/service_worker/service_worker_disk_cache_migrator.cc browser/service_worker/service_worker_dispatcher_host.cc browser/service_worker/service_worker_fetch_dispatcher.cc browser/service_worker/service_worker_handle.cc browser/service_worker/service_worker_info.cc browser/service_worker/service_worker_internals_ui.cc browser/service_worker/service_worker_job_coordinator.cc browser/service_worker/service_worker_metrics.cc browser/service_worker/service_worker_navigation_handle.cc browser/service_worker/service_worker_navigation_handle_core.cc browser/service_worker/service_worker_process_manager.cc browser/service_worker/service_worker_provider_host.cc browser/service_worker/service_worker_quota_client.cc browser/service_worker/service_worker_read_from_cache_job.cc browser/service_worker/service_worker_register_job.cc browser/service_worker/service_worker_registration.cc browser/service_worker/service_worker_registration_handle.cc browser/service_worker/service_worker_registration_status.cc browser/service_worker/service_worker_request_handler.cc browser/service_worker/service_worker_script_cache_map.cc browser/service_worker/service_worker_storage.cc browser/service_worker/service_worker_unregister_job.cc browser/service_worker/service_worker_url_request_job.cc browser/service_worker/service_worker_version.cc browser/service_worker/service_worker_write_to_cache_job.cc)

# browser_shared_worker
ADD_LIBRARY(browser_shared_worker OBJECT browser/shared_worker/shared_worker_host.cc browser/shared_worker/shared_worker_instance.cc browser/shared_worker/shared_worker_message_filter.cc browser/shared_worker/shared_worker_service_impl.cc browser/shared_worker/worker_document_set.cc browser/shared_worker/worker_storage_partition.cc)

# browser_speech
ADD_LIBRARY(browser_speech OBJECT browser/speech/audio_buffer.cc browser/speech/chunked_byte_buffer.cc browser/speech/endpointer/endpointer.cc browser/speech/endpointer/energy_endpointer.cc browser/speech/endpointer/energy_endpointer_params.cc browser/speech/speech_recognition_dispatcher_host.cc browser/speech/speech_recognition_engine.cc browser/speech/speech_recognition_manager_impl.cc browser/speech/speech_recognizer_impl.cc)
# "google_one_shot_remote_engine.cc" and "google_streaming_remote_engine.cc"
# pull "audio_encoder.cc". It then pulls FLAC, which we do not want that.

# browser_ssl
ADD_LIBRARY(browser_ssl OBJECT browser/ssl/ssl_cert_error_handler.cc browser/ssl/ssl_client_auth_handler.cc browser/ssl/ssl_error_handler.cc browser/ssl/ssl_manager.cc browser/ssl/ssl_policy.cc browser/ssl/ssl_policy_backend.cc browser/ssl/ssl_request_info.cc)

# browser_streams
ADD_LIBRARY(browser_streams OBJECT browser/streams/stream.cc browser/streams/stream_context.cc browser/streams/stream_handle_impl.cc browser/streams/stream_registry.cc browser/streams/stream_url_request_job.cc)

# browser_tracing
ADD_LIBRARY(browser_tracing OBJECT browser/tracing/background_tracing_config_impl.cc browser/tracing/background_tracing_manager_impl.cc browser/tracing/background_tracing_rule.cc browser/tracing/battor_power_trace_provider.cc browser/tracing/file_tracing_provider_impl.cc browser/tracing/power_tracing_agent.cc browser/tracing/trace_message_filter.cc browser/tracing/tracing_controller_impl.cc browser/tracing/tracing_controller_impl_data_sinks.cc browser/tracing/tracing_ui.cc)

# browser_wakelock
ADD_LIBRARY(browser_wakelock OBJECT browser/wake_lock/wake_lock_service_context.cc browser/wake_lock/wake_lock_service_impl.cc)

# browser_webcontents
ADD_LIBRARY(browser_webcontents OBJECT browser/web_contents/web_contents_impl.cc browser/web_contents/web_contents_view_guest.cc browser/web_contents/web_contents_view_aura.cc browser/web_contents/aura/gesture_nav_simple.cc browser/web_contents/aura/overscroll_navigation_overlay.cc browser/web_contents/aura/overscroll_window_animation.cc browser/web_contents/aura/overscroll_window_delegate.cc browser/web_contents/aura/shadow_layer_delegate.cc)

# browser_webui
ADD_LIBRARY(browser_webui OBJECT browser/webui/content_web_ui_controller_factory.cc browser/webui/generic_handler.cc browser/webui/shared_resources_data_source.cc browser/webui/url_data_manager.cc browser/webui/url_data_manager_backend.cc browser/webui/url_data_source_impl.cc browser/webui/web_ui_controller_factory_registry.cc browser/webui/web_ui_data_source_impl.cc browser/webui/web_ui_impl.cc browser/webui/web_ui_message_handler.cc)

# child
ADD_LIBRARY(child OBJECT public/child/fixed_received_data.cc child/assert_matching_enums.cc child/blink_platform_impl.cc child/child_discardable_shared_memory_manager.cc child/child_gpu_memory_buffer_manager.cc child/child_histogram_message_filter.cc child/child_message_filter.cc child/child_process.cc child/child_resource_message_filter.cc child/child_shared_bitmap_manager.cc child/child_thread_impl.cc child/content_child_helpers.cc child/database_util.cc child/db_message_filter.cc child/file_info_util.cc child/ftp_directory_listing_response_delegate.cc child/image_decoder.cc child/multipart_response_delegate.cc child/power_monitor_broadcast_source.cc child/process_control_impl.cc child/quota_dispatcher.cc child/quota_message_filter.cc child/request_extra_data.cc child/request_info.cc child/resource_dispatcher.cc child/resource_scheduling_filter.cc child/runtime_features.cc child/scoped_child_process_reference.cc child/shared_memory_data_consumer_handle.cc child/shared_memory_received_data_factory.cc child/shared_worker_devtools_agent.cc child/site_isolation_stats_gatherer.cc child/sync_load_response.cc child/thread_safe_sender.cc child/v8_value_converter_impl.cc child/web_data_consumer_handle_impl.cc child/web_database_observer_impl.cc child/web_discardable_memory_impl.cc child/web_memory_allocator_dump_impl.cc child/web_memory_dump_provider_adapter.cc child/web_process_memory_dump_impl.cc child/web_url_loader_impl.cc child/web_url_request_util.cc child/webblobregistry_impl.cc child/webfallbackthemeengine_impl.cc child/webfileutilities_impl.cc child/webmessageportchannel_impl.cc child/websocket_bridge.cc child/websocket_dispatcher.cc child/webthemeengine_impl_default.cc child/weburlresponse_extradata_impl.cc child/worker_task_runner.cc child/worker_thread_impl.cc child/worker_thread_message_filter.cc)

# child_appcache
ADD_LIBRARY(child_appcache OBJECT child/appcache/appcache_backend_proxy.cc child/appcache/appcache_dispatcher.cc child/appcache/appcache_frontend_impl.cc child/appcache/web_application_cache_host_impl.cc)

# child_background_sync
ADD_LIBRARY(child_background_sync OBJECT child/background_sync/background_sync_provider.cc child/background_sync/background_sync_type_converters.cc)

# child_blob_storage
ADD_LIBRARY(child_blob_storage OBJECT child/blob_storage/blob_consolidation.cc child/blob_storage/blob_transport_controller.cc)

# child_fileapi
ADD_LIBRARY(child_fileapi OBJECT child/fileapi/file_system_dispatcher.cc child/fileapi/webfilesystem_impl.cc child/fileapi/webfilewriter_base.cc child/fileapi/webfilewriter_impl.cc)

# child_indexeddb
ADD_LIBRARY(child_indexeddb OBJECT child/indexed_db/indexed_db_dispatcher.cc child/indexed_db/indexed_db_key_builders.cc child/indexed_db/indexed_db_message_filter.cc child/indexed_db/webidbcursor_impl.cc child/indexed_db/webidbdatabase_impl.cc child/indexed_db/webidbfactory_impl.cc)

# child_memory
ADD_LIBRARY(child_memory OBJECT child/memory/child_memory_message_filter.cc)

# child_mojo
ADD_LIBRARY(child_mojo OBJECT child/mojo/mojo_application.cc)

# child_notifications
ADD_LIBRARY(child_notifications OBJECT child/notifications/notification_data_conversions.cc child/notifications/notification_dispatcher.cc child/notifications/notification_image_loader.cc child/notifications/notification_manager.cc child/notifications/pending_notifications_tracker.cc)

# child_push_messaging
ADD_LIBRARY(child_push_messaging OBJECT child/push_messaging/push_dispatcher.cc child/push_messaging/push_provider.cc)

# child_service_worker
ADD_LIBRARY(child_service_worker OBJECT child/service_worker/service_worker_dispatcher.cc child/service_worker/service_worker_handle_reference.cc child/service_worker/service_worker_message_filter.cc child/service_worker/service_worker_network_provider.cc child/service_worker/service_worker_provider_context.cc child/service_worker/service_worker_registration_handle_reference.cc child/service_worker/web_service_worker_impl.cc child/service_worker/web_service_worker_provider_impl.cc child/service_worker/web_service_worker_registration_impl.cc)

# renderer
ADD_LIBRARY(renderer OBJECT public/renderer/browser_plugin_delegate.cc public/renderer/chrome_object_extensions_utils.cc public/renderer/content_renderer_client.cc public/renderer/document_state.cc public/renderer/navigation_state.cc public/renderer/render_frame_observer.cc public/renderer/render_process_observer.cc public/renderer/render_thread.cc public/renderer/render_view_observer.cc public/renderer/video_encode_accelerator.cc renderer/child_frame_compositing_helper.cc renderer/clipboard_utils.cc renderer/context_menu_params_builder.cc renderer/cursor_utils.cc renderer/disambiguation_popup_helper.cc renderer/dom_automation_controller.cc renderer/drop_data_builder.cc renderer/gamepad_shared_memory_reader.cc renderer/history_controller.cc renderer/history_entry.cc renderer/history_serialization.cc renderer/idle_user_detector.cc renderer/ime_event_guard.cc renderer/in_process_renderer_thread.cc renderer/internal_document_state_data.cc renderer/menu_item_builder.cc renderer/mouse_lock_dispatcher.cc renderer/navigation_state_impl.cc renderer/net_info_helper.cc renderer/notification_permission_dispatcher.cc renderer/peripheral_content_heuristic.cc renderer/raster_worker_pool.cc renderer/render_frame_impl.cc renderer/render_frame_impl.cc renderer/render_frame_proxy.cc renderer/render_process_impl.cc renderer/render_thread_impl.cc renderer/render_view_impl.cc renderer/render_view_linux.cc renderer/render_view_mouse_lock_dispatcher.cc renderer/render_widget.cc renderer/render_widget_fullscreen.cc renderer/renderer_blink_platform_impl.cc renderer/renderer_clipboard_delegate.cc renderer/renderer_main.cc renderer/renderer_main_platform_delegate_linux.cc renderer/renderer_webapplicationcachehost_impl.cc renderer/renderer_webcolorchooser_impl.cc renderer/renderer_webcookiejar_impl.cc renderer/resizing_mode_selector.cc renderer/sad_plugin.cc renderer/savable_resources.cc renderer/shared_memory_seqlock_reader.cc renderer/speech_recognition_dispatcher.cc renderer/stats_collection_controller.cc renderer/stats_collection_observer.cc renderer/text_input_client_observer.cc renderer/web_frame_utils.cc renderer/web_ui_extension.cc renderer/web_ui_extension_data.cc renderer/webclipboard_impl.cc renderer/webgraphicscontext3d_provider_impl.cc renderer/webpublicsuffixlist_impl.cc renderer/webscrollbarbehavior_impl_gtkoraura.cc renderer/websharedworker_proxy.cc)
# "renderer/memory_benchmarking_extension.cc" is disabled

# renderer_accessibility
ADD_LIBRARY(renderer_accessibility OBJECT renderer/accessibility/blink_ax_enum_conversion.cc renderer/accessibility/blink_ax_tree_source.cc renderer/accessibility/renderer_accessibility.cc)

# renderer_background_sync
ADD_LIBRARY(renderer_background_sync OBJECT renderer/background_sync/background_sync_client_impl.cc)

# renderer_bluetooth
#ADD_LIBRARY(renderer_bluetooth OBJECT renderer/bluetooth/bluetooth_dispatcher.cc renderer/bluetooth/bluetooth_message_filter.cc)
# Bluetooth DISABLED !

# renderer_browser_plugin
ADD_LIBRARY(renderer_browser_plugin OBJECT renderer/browser_plugin/browser_plugin.cc renderer/browser_plugin/browser_plugin_manager.cc)

# renderer_cache_storage
ADD_LIBRARY(renderer_cache_storage OBJECT renderer/cache_storage/cache_storage_dispatcher.cc renderer/cache_storage/cache_storage_dispatcher.cc renderer/cache_storage/cache_storage_message_filter.cc renderer/cache_storage/webserviceworkercachestorage_impl.cc)

# renderer_device_sensors
ADD_LIBRARY(renderer_device_sensors OBJECT renderer/device_sensors/device_light_event_pump.cc renderer/device_sensors/device_motion_event_pump.cc renderer/device_sensors/device_orientation_absolute_event_pump.cc renderer/device_sensors/device_orientation_event_pump.cc)

# renderer_devtools
ADD_LIBRARY(renderer_devtools OBJECT renderer/devtools/devtools_agent.cc renderer/devtools/devtools_agent_filter.cc renderer/devtools/devtools_client.cc renderer/devtools/devtools_cpu_throttler.cc renderer/devtools/v8_sampling_profiler.cc)

# renderer_dom_storage
ADD_LIBRARY(renderer_dom_storage OBJECT renderer/dom_storage/dom_storage_cached_area.cc renderer/dom_storage/dom_storage_dispatcher.cc renderer/dom_storage/webstoragearea_impl.cc renderer/dom_storage/webstoragenamespace_impl.cc)

# renderer_fetchers
ADD_LIBRARY(renderer_fetchers OBJECT renderer/fetchers/manifest_fetcher.cc renderer/fetchers/multi_resolution_image_resource_fetcher.cc renderer/fetchers/resource_fetcher_impl.cc renderer/fetchers/web_url_loader_client_impl.cc)

# renderer_gpu
ADD_LIBRARY(renderer_gpu OBJECT renderer/gpu/compositor_external_begin_frame_source.cc renderer/gpu/compositor_forwarding_message_filter.cc renderer/gpu/renderer_compositor_frame_sink.cc renderer/gpu/delegated_compositor_output_surface.cc renderer/gpu/frame_swap_message_queue.cc renderer/gpu/mailbox_output_surface.cc renderer/gpu/queue_message_swap_promise.cc renderer/gpu/render_widget_compositor.cc renderer/gpu/renderer_compositor_frame_sink.cc)

# renderer_image_downloader
ADD_LIBRARY(renderer_image_downloader OBJECT renderer/image_downloader/image_downloader_impl.cc)

# renderer_input
ADD_LIBRARY(renderer_input OBJECT renderer/input/input_event_filter.cc renderer/input/input_handler_manager.cc renderer/input/input_handler_wrapper.cc renderer/input/main_thread_input_event_filter.cc renderer/input/render_widget_input_handler.cc)

# renderer_manifest
ADD_LIBRARY(renderer_manifest OBJECT renderer/manifest/manifest_manager.cc renderer/manifest/manifest_parser.cc renderer/manifest/manifest_uma_util.cc)

# renderer_media
ADD_LIBRARY(renderer_media OBJECT renderer/media/aec_dump_message_filter.cc renderer/media/audio_device_factory.cc renderer/media/audio_input_message_filter.cc renderer/media/audio_message_filter.cc renderer/media/audio_renderer_mixer_manager.cc renderer/media/media_permission_dispatcher.cc renderer/media/media_permission_dispatcher_impl.cc renderer/media/media_permission_dispatcher_proxy.cc renderer/media/media_stream_audio_level_calculator.cc renderer/media/media_stream_audio_track.cc renderer/media/media_stream_constraints_util.cc renderer/media/media_stream_track.cc renderer/media/midi_message_filter.cc renderer/media/render_media_client.cc renderer/media/render_media_log.cc renderer/media/renderer_gpu_video_accelerator_factories.cc renderer/media/renderer_webaudiodevice_impl.cc renderer/media/renderer_webmediaplayer_delegate.cc renderer/media/renderer_webmidiaccessor_impl.cc renderer/media/video_capture_impl.cc renderer/media/video_capture_impl_manager.cc renderer/media/video_capture_message_filter.cc renderer/media/webmediaplayer_ms.cc renderer/media/webmediaplayer_ms_compositor.cc renderer/media/cdm/render_cdm_factory.cc renderer/media/webrtc_logging_noop.cc) 
# "audio_decoder.cc" is disabled because is pull a FFmpeg-dependent part
# of "media"
# "cdm/pepper_cdm_wrapper_impl.cc" and "cdm/ppapi_decryptor.cc" are disabled
# because they pull PPAPI

# renderer_mojo
ADD_LIBRARY(renderer_mojo OBJECT renderer/mojo/service_registry_js_wrapper.cc renderer/mojo_bindings_controller.cc renderer/mojo_context_state.cc renderer/mojo_main_runner.cc)

# renderer_presentation
ADD_LIBRARY(renderer_presentation OBJECT renderer/presentation/presentation_connection_client.cc renderer/presentation/presentation_dispatcher.cc)

# renderer_push_messaging
ADD_LIBRARY(renderer_push_messaging OBJECT renderer/push_messaging/push_messaging_dispatcher.cc)

# renderer_scheduler
ADD_LIBRARY(renderer_scheduler OBJECT renderer/scheduler/resource_dispatch_throttler.cc)

# renderer_screen_orientation
ADD_LIBRARY(renderer_screen_orientation OBJECT renderer/screen_orientation/screen_orientation_dispatcher.cc renderer/screen_orientation/screen_orientation_observer.cc)

# renderer_service_worker
ADD_LIBRARY(renderer_service_worker OBJECT renderer/service_worker/embedded_worker_devtools_agent.cc renderer/service_worker/embedded_worker_dispatcher.cc renderer/service_worker/service_worker_context_client.cc renderer/service_worker/service_worker_context_message_filter.cc renderer/service_worker/service_worker_type_util.cc)

# renderer_shared_worker
ADD_LIBRARY(renderer_shared_worker OBJECT renderer/shared_worker/embedded_shared_worker_content_settings_client_proxy.cc renderer/shared_worker/embedded_shared_worker_stub.cc renderer/shared_worker_repository.cc)

# renderer_vr
#ADD_LIBRARY(renderer_vr OBJECT renderer/vr/vr_dispatcher.cc renderer/vr/vr_type_converters.cc)
# WebVR DISABLED ; this would require -DENABLE_WEBVR

# renderer_wakelock
ADD_LIBRARY(renderer_wakelock OBJECT renderer/wake_lock/wake_lock_dispatcher.cc)

# app
ADD_LIBRARY(app OBJECT app/content_main.cc app/content_main_runner.cc app/mojo/mojo_init.cc public/app/content_main_delegate.cc)

# gpu
ADD_LIBRARY(gpu OBJECT gpu/gpu_child_thread.cc gpu/gpu_main.cc gpu/gpu_process.cc gpu/gpu_process_control_impl.cc gpu/gpu_watchdog_thread.cc gpu/in_process_gpu_thread.cc)

# utility
ADD_LIBRARY(utility OBJECT public/utility/content_utility_client.cc public/utility/utility_thread.cc utility/in_process_utility_thread.cc utility/utility_blink_platform_impl.cc utility/utility_main.cc utility/utility_process_control_impl.cc utility/utility_thread_impl.cc utility/webthread_impl_for_utility_thread.cc)

# pepper
# ADD_LIBRARY(pepper OBJECT public/common/pepper_plugin_info.cc common/pepper_file_util.cc common/pepper_plugin_list.cc common/pepper_render_instance_data.cc common/plugin_list.cc common/plugin_list_posix.cc common/sandbox_util.cc)
# Pepper (PPAPI) disabled here !

# sandbox_linux
ADD_LIBRARY(sandbox_linux OBJECT common/sandbox_linux/sandbox_debug_handling_linux.cc common/sandbox_linux/sandbox_init_linux.cc common/sandbox_linux/sandbox_linux.cc common/sandbox_linux/sandbox_seccomp_bpf_linux.cc)
# common/sandbox_linux/bpf_cros_arm_gpu_policy_linux.cc common/sandbox_linux/bpf_gpu_policy_linux.cc common/sandbox_linux/bpf_ppapi_policy_linux.cc common/sandbox_linux/bpf_renderer_policy_linux.cc common/sandbox_linux/bpf_utility_policy_linux.cc common/sandbox_linux/sandbox_bpf_base_policy_linux.cc
# ADD_DEFINITIONS(-DUSE_SECCOMP_BPF)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/content/common/routed_service_provider.mojom.cc ${CMAKE_BINARY_DIR}/content/common/presentation/presentation_service.mojom.cc ${CMAKE_BINARY_DIR}/content/common/wake_lock_service.mojom.cc ${CMAKE_BINARY_DIR}/content/common/application_setup.mojom.cc ${CMAKE_BINARY_DIR}/content/common/process_control.mojom.cc ${CMAKE_BINARY_DIR}/content/common/image_downloader/image_downloader.mojom.cc ${CMAKE_BINARY_DIR}/content/common/service_worker/embedded_worker_setup.mojom.cc ${CMAKE_BINARY_DIR}/content/common/geolocation_service.mojom.cc ${CMAKE_BINARY_DIR}/content/common/permission_service.mojom.cc ${CMAKE_BINARY_DIR}/content/public/common/service_worker_event_status.mojom.cc ${CMAKE_BINARY_DIR}/content/public/common/permission_status.mojom.cc ${CMAKE_BINARY_DIR}/content/public/common/mojo_geoposition.mojom.cc ${CMAKE_BINARY_DIR}/content/public/common/background_sync.mojom.cc ${CMAKE_BINARY_DIR}/content/common/background_sync_service.mojom.cc ${CMAKE_BINARY_DIR}/content/browser/background_sync/background_sync.pb.cc ${CMAKE_BINARY_DIR}/content/browser/cache_storage/cache_storage.pb.cc ${CMAKE_BINARY_DIR}/content/browser/notifications/notification_database_data.pb.cc ${CMAKE_BINARY_DIR}/content/browser/service_worker/service_worker_database.pb.cc ${CMAKE_BINARY_DIR}/content/browser/speech/proto/google_streaming_api.pb.cc ${CMAKE_BINARY_DIR}/content/browser/devtools/protocol/devtools_protocol_dispatcher.cc ${CMAKE_BINARY_DIR}/blink/grit/devtools_resources_map.cc)


# all files refer to "content/...", so we include ".." here
# we put LEVELDB_EXT_DIR first, because the fork has specific required headers/values
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SRC_DIR}/include/core ${SKIA_SRC_DIR}/include/utils ${SKIA_SRC_DIR}/include/gpu ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${CC_DIR} ${BLINK_DIR} ${NPAPI_DIR} ${V8_DIR}/include ${MOJO_SRC_DIR}/.. ${WEBRTC_DIR}/.. ${WIDEVINE_DIR}/cdm ${ANGLE_SRC_DIR}/src ${LEVELDB_EXT_DIR}/src ${LEVELDB_EXT_DIR}/src/include ${deps_INCLUDE_DIRS} ${nss_INCLUDE_DIRS} ${minizip_INCLUDE_DIRS} ${MOJO_SRC_DIR}/Build ${MOJO_SRC_DIR}/Build/mojo ${SKIA_SRC_DIR}/Build ${SKIA_SRC_DIR}/Build/skia ${UI_STRINGS_DIR}/Build ${UI_RESOURCES_DIR}/Build ${UI_ACCESSIBILITY_DIR}/Build ${UI_GFX_DIR}/Build ${UI_GFX_DIR}/Build/ui ${CC_DIR}/Build ${BLINK_DIR}/Source/Build ${CMAKE_BINARY_DIR}/content ${CMAKE_BINARY_DIR}/blink ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/..)
# TODO : "${MOJO_SRC_DIR}/${UI_ACCESSIBILITY_DIR}/${BLINK_DIR}/Build" are HACKS !
SET(link_libraries ${LINK_LIBRARIES} ${deps_LIBRARIES} ${nss_LIBRARIES} ${minizip_LIBRARIES} ${backend_LIBRARIES})

ADD_LIBRARY(content SHARED $<TARGET_OBJECTS:startup_metrics> $<TARGET_OBJECTS:url_formatter> $<TARGET_OBJECTS:scheduler> $<TARGET_OBJECTS:tracing> $<TARGET_OBJECTS:webcrypto> $<TARGET_OBJECTS:sandbox> $<TARGET_OBJECTS:google_apis> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:common_cache_storage> $<TARGET_OBJECTS:common_cursors> $<TARGET_OBJECTS:common_dom_storage> $<TARGET_OBJECTS:common_gpu> $<TARGET_OBJECTS:common_gpu_client> $<TARGET_OBJECTS:common_gpu_media> $<TARGET_OBJECTS:common_indexeddb> $<TARGET_OBJECTS:common_input> $<TARGET_OBJECTS:common_media> $<TARGET_OBJECTS:common_mojo> $<TARGET_OBJECTS:common_net> $<TARGET_OBJECTS:common_plugin> $<TARGET_OBJECTS:common_service_worker> $<TARGET_OBJECTS:browser> $<TARGET_OBJECTS:browser_accessibility> $<TARGET_OBJECTS:browser_appcache> $<TARGET_OBJECTS:browser_background_sync> $<TARGET_OBJECTS:browser_plugin> $<TARGET_OBJECTS:browser_cache_storage> $<TARGET_OBJECTS:browser_compositor> $<TARGET_OBJECTS:browser_device_sensors> $<TARGET_OBJECTS:browser_devtools> $<TARGET_OBJECTS:browser_dom_storage> $<TARGET_OBJECTS:browser_download> $<TARGET_OBJECTS:browser_fileapi> $<TARGET_OBJECTS:browser_framehost> $<TARGET_OBJECTS:browser_gamepad> $<TARGET_OBJECTS:browser_geolocation> $<TARGET_OBJECTS:browser_gpu> $<TARGET_OBJECTS:browser_indexeddb> $<TARGET_OBJECTS:browser_loader> $<TARGET_OBJECTS:browser_media> $<TARGET_OBJECTS:browser_memory> $<TARGET_OBJECTS:browser_mojo> $<TARGET_OBJECTS:browser_net> $<TARGET_OBJECTS:browser_notifications> $<TARGET_OBJECTS:browser_permissions> $<TARGET_OBJECTS:browser_presentation> $<TARGET_OBJECTS:browser_push_messaging> $<TARGET_OBJECTS:browser_renderer> $<TARGET_OBJECTS:browser_service_worker> $<TARGET_OBJECTS:browser_shared_worker> $<TARGET_OBJECTS:browser_speech> $<TARGET_OBJECTS:browser_ssl> $<TARGET_OBJECTS:browser_streams> $<TARGET_OBJECTS:browser_tracing> $<TARGET_OBJECTS:browser_wakelock> $<TARGET_OBJECTS:browser_webcontents> $<TARGET_OBJECTS:browser_webui> $<TARGET_OBJECTS:child> $<TARGET_OBJECTS:child_appcache> $<TARGET_OBJECTS:child_background_sync> $<TARGET_OBJECTS:child_blob_storage> $<TARGET_OBJECTS:child_fileapi> $<TARGET_OBJECTS:child_indexeddb> $<TARGET_OBJECTS:child_memory> $<TARGET_OBJECTS:child_mojo> $<TARGET_OBJECTS:child_notifications> $<TARGET_OBJECTS:child_push_messaging> $<TARGET_OBJECTS:child_service_worker> $<TARGET_OBJECTS:renderer> $<TARGET_OBJECTS:renderer_accessibility> $<TARGET_OBJECTS:renderer_background_sync> $<TARGET_OBJECTS:renderer_browser_plugin> $<TARGET_OBJECTS:renderer_cache_storage> $<TARGET_OBJECTS:renderer_device_sensors> $<TARGET_OBJECTS:renderer_devtools> $<TARGET_OBJECTS:renderer_dom_storage> $<TARGET_OBJECTS:renderer_fetchers> $<TARGET_OBJECTS:renderer_gpu> $<TARGET_OBJECTS:renderer_image_downloader> $<TARGET_OBJECTS:renderer_input> $<TARGET_OBJECTS:renderer_manifest> $<TARGET_OBJECTS:renderer_media> $<TARGET_OBJECTS:renderer_mojo> $<TARGET_OBJECTS:renderer_presentation> $<TARGET_OBJECTS:renderer_push_messaging> $<TARGET_OBJECTS:renderer_scheduler> $<TARGET_OBJECTS:renderer_screen_orientation> $<TARGET_OBJECTS:renderer_service_worker> $<TARGET_OBJECTS:renderer_shared_worker> $<TARGET_OBJECTS:renderer_wakelock> $<TARGET_OBJECTS:app> $<TARGET_OBJECTS:gpu> $<TARGET_OBJECTS:utility> $<TARGET_OBJECTS:sandbox_linux> $<TARGET_OBJECTS:generated> ${BACKEND_SOURCE_FILES} ${MIME_UTIL_DIR}/mime_util.cc ${ZLIB_EXT_DIR}/google/zip.cc ${ZLIB_EXT_DIR}/google/zip_internal.cc ${ZLIB_EXT_DIR}/google/zip_reader.cc)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(content ${link_libraries})

INSTALL(TARGETS content
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
