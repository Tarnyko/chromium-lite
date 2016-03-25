PROJECT(chromium-cc CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - CC")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Variable paths for required third-parties
SET(PROTOC_WRAPPER_DIR "..")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../gpu")

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
PKG_CHECK_MODULES(protobuf REQUIRED protobuf)

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

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DCC_IMPLEMENTATION=1 -DCC_PROTO_IMPLEMENTATION=1 -DCC_SURFACES_IMPLEMENTATION=1 -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .PROTO -> .CC,.H,.PY

MESSAGE(STATUS "Generating .cc,.h files from .proto...")

EXECUTE_PROCESS(COMMAND mkdir -p cc/proto WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/compositor_message.pb.h --proto-in-dir proto --proto-in-file compositor_message.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/display_item.pb.h --proto-in-dir proto --proto-in-file display_item.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/display_list_recording_source.pb.h --proto-in-dir proto --proto-in-file display_list_recording_source.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/layer.pb.h --proto-in-dir proto --proto-in-file layer.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/layer_position_constraint.pb.h --proto-in-dir proto --proto-in-file layer_position_constraint.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/layer_tree_debug_state.pb.h --proto-in-dir proto --proto-in-file layer_tree_debug_state.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/layer_tree_settings.pb.h --proto-in-dir proto --proto-in-file layer_tree_settings.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/managed_memory_policy.pb.h --proto-in-dir proto --proto-in-file managed_memory_policy.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/memory_allocation.pb.h --proto-in-dir proto --proto-in-file memory_allocation.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/point.pb.h --proto-in-dir proto --proto-in-file point.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/point3f.pb.h --proto-in-dir proto --proto-in-file point3f.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/pointf.pb.h --proto-in-dir proto --proto-in-file pointf.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --protobuf ${CMAKE_BINARY_DIR}/cc/proto/property_tree.pb.h --include proto/cc_proto_export.h --proto-in-dir proto --proto-in-file property_tree.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/rect.pb.h --proto-in-dir proto --proto-in-file rect.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/rectf.pb.h --proto-in-dir proto --proto-in-file rectf.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/region.pb.h --proto-in-dir proto --proto-in-file region.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/renderer_settings.pb.h --proto-in-dir proto --proto-in-file renderer_settings.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/scroll_offset.pb.h --proto-in-dir proto --proto-in-file scroll_offset.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/size.pb.h --proto-in-dir proto --proto-in-file size.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/sizef.pb.h --proto-in-dir proto --proto-in-file sizef.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/skregion.pb.h --proto-in-dir proto --proto-in-file skregion.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/skrrect.pb.h --proto-in-dir proto --proto-in-file skrrect.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/skxfermode.pb.h --proto-in-dir proto --proto-in-file skxfermode.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/transform.pb.h --proto-in-dir proto --proto-in-file transform.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include proto/cc_proto_export.h --protobuf ${CMAKE_BINARY_DIR}/cc/proto/vector2df.pb.h --proto-in-dir proto --proto-in-file vector2df.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=CC_PROTO_EXPORT:${CMAKE_BINARY_DIR}/cc/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# animation
ADD_LIBRARY(animation OBJECT animation/animation.cc animation/animation_curve.cc animation/animation_events.cc animation/animation_host.cc animation/animation_id_provider.cc animation/animation_player.cc animation/animation_registrar.cc animation/animation_timeline.cc animation/element_animations.cc animation/keyframed_animation_curve.cc animation/layer_animation_controller.cc animation/scroll_offset_animation_curve.cc animation/scrollbar_animation_controller.cc animation/scrollbar_animation_controller_linear_fade.cc animation/scrollbar_animation_controller_thinning.cc animation/timing_function.cc animation/transform_operation.cc animation/transform_operations.cc)

# base
ADD_LIBRARY(base OBJECT base/contiguous_container.cc base/delayed_unique_notifier.cc base/histograms.cc base/invalidation_region.cc base/list_container_helper.cc base/math_util.cc base/region.cc base/rolling_time_delta_history.cc base/rtree.cc base/simple_enclosed_region.cc base/switches.cc base/tiling_data.cc base/unique_notifier.cc)

# debug
ADD_LIBRARY(debug OBJECT debug/benchmark_instrumentation.cc debug/debug_colors.cc debug/debug_rect_history.cc debug/frame_rate_counter.cc debug/frame_timing_request.cc debug/frame_timing_tracker.cc debug/frame_viewer_instrumentation.cc debug/invalidation_benchmark.cc debug/lap_timer.cc debug/layer_tree_debug_state.cc debug/micro_benchmark.cc debug/micro_benchmark_controller.cc debug/micro_benchmark_controller_impl.cc debug/micro_benchmark_impl.cc debug/picture_debug_util.cc debug/rasterize_and_record_benchmark.cc debug/rasterize_and_record_benchmark_impl.cc debug/rendering_stats.cc debug/rendering_stats_instrumentation.cc debug/traced_display_item_list.cc debug/traced_value.cc debug/unittest_only_benchmark.cc debug/unittest_only_benchmark_impl.cc)

# input
ADD_LIBRARY(input OBJECT input/input_handler.cc input/layer_selection_bound.cc input/page_scale_animation.cc input/scroll_elasticity_helper.cc input/scroll_state.cc input/scroll_state_data.cc input/top_controls_manager.cc)

# layers
ADD_LIBRARY(layers OBJECT layers/delegated_frame_provider.cc layers/delegated_frame_resource_collection.cc layers/delegated_renderer_layer.cc layers/delegated_renderer_layer_impl.cc layers/draw_properties.cc layers/heads_up_display_layer.cc layers/heads_up_display_layer_impl.cc layers/io_surface_layer.cc layers/io_surface_layer_impl.cc layers/layer.cc layers/layer_impl.cc layers/layer_position_constraint.cc layers/layer_proto_converter.cc layers/layer_settings.cc layers/layer_utils.cc layers/nine_patch_layer.cc layers/nine_patch_layer_impl.cc layers/painted_scrollbar_layer.cc layers/painted_scrollbar_layer_impl.cc layers/picture_image_layer.cc layers/picture_image_layer_impl.cc layers/picture_layer.cc layers/picture_layer_impl.cc layers/render_surface_draw_properties.cc layers/render_surface_impl.cc layers/scrollbar_layer_impl_base.cc layers/solid_color_layer.cc layers/solid_color_layer_impl.cc layers/solid_color_scrollbar_layer.cc layers/solid_color_scrollbar_layer_impl.cc layers/surface_layer.cc layers/surface_layer_impl.cc layers/texture_layer.cc layers/texture_layer_impl.cc layers/ui_resource_layer.cc layers/ui_resource_layer_impl.cc layers/video_frame_provider_client_impl.cc layers/video_layer.cc layers/video_layer_impl.cc layers/viewport.cc)

# output
ADD_LIBRARY(output OBJECT output/begin_frame_args.cc output/bsp_tree.cc output/bsp_walk_action.cc output/ca_layer_overlay.cc output/compositor_frame.cc output/compositor_frame_ack.cc output/compositor_frame_metadata.cc output/context_provider.cc output/copy_output_request.cc output/copy_output_result.cc output/delegated_frame_data.cc output/delegating_renderer.cc output/direct_renderer.cc output/dynamic_geometry_binding.cc output/filter_operation.cc output/filter_operations.cc output/geometry_binding.cc output/gl_frame_data.cc output/gl_renderer.cc output/gl_renderer_draw_cache.cc output/latency_info_swap_promise.cc output/layer_quad.cc output/managed_memory_policy.cc output/output_surface.cc output/overlay_candidate.cc output/overlay_processor.cc output/overlay_strategy_sandwich.cc output/overlay_strategy_single_on_top.cc output/overlay_strategy_underlay.cc output/program_binding.cc output/render_surface_filters.cc output/renderer.cc output/renderer_capabilities.cc output/renderer_settings.cc output/shader.cc output/software_output_device.cc output/software_renderer.cc output/static_geometry_binding.cc output/texture_mailbox_deleter.cc output/viewport_selection_bound.cc)

# playback
ADD_LIBRARY(playback OBJECT playback/clip_display_item.cc playback/clip_path_display_item.cc playback/compositing_display_item.cc playback/discardable_image_map.cc playback/display_item.cc playback/display_item_list.cc playback/display_item_list_settings.cc playback/display_item_proto_factory.cc playback/display_list_raster_source.cc playback/display_list_recording_source.cc playback/drawing_display_item.cc playback/filter_display_item.cc playback/float_clip_display_item.cc playback/largest_display_item.cc playback/transform_display_item.cc)

# proto
ADD_LIBRARY(proto OBJECT proto/cc_conversions.cc proto/gfx_conversions.cc proto/gpu_conversions.cc proto/skia_conversions.cc)

# quads
ADD_LIBRARY(quads OBJECT quads/content_draw_quad_base.cc quads/debug_border_draw_quad.cc quads/draw_polygon.cc quads/draw_quad.cc quads/io_surface_draw_quad.cc quads/largest_draw_quad.cc quads/picture_draw_quad.cc quads/render_pass.cc quads/render_pass_draw_quad.cc quads/render_pass_id.cc quads/shared_quad_state.cc quads/solid_color_draw_quad.cc quads/stream_video_draw_quad.cc quads/surface_draw_quad.cc quads/texture_draw_quad.cc quads/tile_draw_quad.cc quads/yuv_video_draw_quad.cc)

# raster
ADD_LIBRARY(raster OBJECT raster/bitmap_tile_task_worker_pool.cc raster/gpu_rasterizer.cc raster/gpu_tile_task_worker_pool.cc raster/one_copy_tile_task_worker_pool.cc raster/raster_buffer.cc raster/scoped_gpu_raster.cc raster/single_thread_task_graph_runner.cc raster/synchronous_task_graph_runner.cc raster/task_graph_runner.cc raster/task_graph_work_queue.cc raster/texture_compressor.cc raster/texture_compressor_etc1.cc raster/tile_task_runner.cc raster/tile_task_worker_pool.cc raster/zero_copy_tile_task_worker_pool.cc)

# resources
ADD_LIBRARY(resources OBJECT resources/memory_history.cc resources/resource_format.cc resources/resource_pool.cc resources/resource_provider.cc resources/scoped_resource.cc resources/scoped_ui_resource.cc resources/shared_bitmap.cc resources/single_release_callback.cc resources/single_release_callback_impl.cc resources/texture_mailbox.cc resources/transferable_resource.cc resources/ui_resource_bitmap.cc resources/ui_resource_request.cc resources/video_resource_updater.cc)

# scheduler
ADD_LIBRARY(scheduler OBJECT scheduler/begin_frame_source.cc scheduler/begin_frame_tracker.cc scheduler/compositor_timing_history.cc scheduler/delay_based_time_source.cc scheduler/scheduler.cc scheduler/scheduler_settings.cc scheduler/scheduler_state_machine.cc)

# tiles
ADD_LIBRARY(tiles OBJECT tiles/eviction_tile_priority_queue.cc tiles/image_decode_controller.cc tiles/picture_layer_tiling.cc tiles/picture_layer_tiling_set.cc tiles/prioritized_tile.cc tiles/raster_tile_priority_queue.cc tiles/raster_tile_priority_queue_all.cc tiles/raster_tile_priority_queue_required.cc tiles/tile.cc tiles/tile_draw_info.cc tiles/tile_manager.cc tiles/tile_priority.cc tiles/tiling_set_eviction_queue.cc tiles/tiling_set_raster_queue_all.cc tiles/tiling_set_raster_queue_required.cc)

# trees
ADD_LIBRARY(trees OBJECT trees/blocking_task_runner.cc trees/damage_tracker.cc trees/draw_property_utils.cc trees/latency_info_swap_promise_monitor.cc trees/layer_tree_host.cc trees/layer_tree_host_common.cc trees/layer_tree_host_impl.cc trees/layer_tree_impl.cc trees/layer_tree_settings.cc trees/occlusion.cc trees/occlusion_tracker.cc trees/property_tree.cc trees/property_tree_builder.cc trees/proxy_common.cc trees/proxy_impl.cc trees/proxy_main.cc trees/single_thread_proxy.cc trees/swap_promise_monitor.cc trees/task_runner_provider.cc trees/threaded_channel.cc trees/tree_synchronizer.cc)

# surfaces
ADD_LIBRARY(surfaces OBJECT surfaces/display.cc surfaces/display_scheduler.cc surfaces/onscreen_display_client.cc surfaces/surface.cc surfaces/surface_aggregator.cc surfaces/surface_display_output_surface.cc surfaces/surface_factory.cc surfaces/surface_hittest.cc surfaces/surface_id_allocator.cc surfaces/surface_manager.cc surfaces/surface_resource_holder.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/cc/proto/compositor_message.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/display_item.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/display_list_recording_source.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/layer.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/layer_position_constraint.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/layer_tree_debug_state.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/layer_tree_settings.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/managed_memory_policy.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/memory_allocation.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/point.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/point3f.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/pointf.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/property_tree.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/rect.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/rectf.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/region.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/renderer_settings.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/scroll_offset.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/size.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/sizef.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/skregion.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/skrrect.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/skxfermode.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/transform.pb.cc ${CMAKE_BINARY_DIR}/cc/proto/vector2df.pb.cc)


# all files refer to "cc/...", so we include "../" here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${protobuf_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/gpu ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR})
SET(link_libraries ${LINK_LIBRARIES} ${protobuf_LIBRARIES})

ADD_LIBRARY(cc SHARED $<TARGET_OBJECTS:animation> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:debug> $<TARGET_OBJECTS:input> $<TARGET_OBJECTS:layers> $<TARGET_OBJECTS:output> $<TARGET_OBJECTS:playback> $<TARGET_OBJECTS:proto> $<TARGET_OBJECTS:quads> $<TARGET_OBJECTS:raster> $<TARGET_OBJECTS:resources> $<TARGET_OBJECTS:scheduler> $<TARGET_OBJECTS:tiles> $<TARGET_OBJECTS:trees> $<TARGET_OBJECTS:surfaces> $<TARGET_OBJECTS:generated> ${TOOLCHAIN_OPTS_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(cc ${link_libraries})

INSTALL(TARGETS cc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
