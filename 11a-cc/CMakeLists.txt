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

# Location of libyuv source folders
SET(LIBYUV_DIR "${CMAKE_SOURCE_DIR}/../third_party/libyuv")

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
ADD_DEFINITIONS(-DCC_IMPLEMENTATION=1 -DCC_PROTO_IMPLEMENTATION=1 -DCC_SURFACES_IMPLEMENTATION=1 -DUSE_AURA -D__STDC_FORMAT_MACROS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# animation
ADD_LIBRARY(animation OBJECT animation/animation.cc animation/animation_curve.cc animation/animation_events.cc animation/animation_host.cc animation/animation_id_provider.cc animation/animation_player.cc animation/animation_timeline.cc animation/element_animations.cc animation/keyframed_animation_curve.cc animation/scroll_offset_animations.cc animation/scroll_offset_animation_curve.cc animation/scroll_offset_animations_impl.cc animation/timing_function.cc animation/transform_operation.cc animation/transform_operations.cc)

# base
ADD_LIBRARY(base OBJECT base/contiguous_container.cc base/delayed_unique_notifier.cc base/histograms.cc base/index_rect.cc base/invalidation_region.cc base/list_container_helper.cc base/math_util.cc base/region.cc base/reverse_spiral_iterator.cc base/rolling_time_delta_history.cc base/rtree.cc base/simple_enclosed_region.cc base/spiral_iterator.cc base/switches.cc base/tiling_data.cc base/unique_notifier.cc)

# debug
ADD_LIBRARY(debug OBJECT debug/benchmark_instrumentation.cc debug/debug_colors.cc debug/debug_rect_history.cc debug/devtools_instrumentation.cc debug/frame_rate_counter.cc debug/frame_viewer_instrumentation.cc debug/invalidation_benchmark.cc debug/lap_timer.cc debug/layer_tree_debug_state.cc debug/micro_benchmark.cc debug/micro_benchmark_controller.cc debug/micro_benchmark_controller_impl.cc debug/micro_benchmark_impl.cc debug/picture_debug_util.cc debug/rasterize_and_record_benchmark.cc debug/rasterize_and_record_benchmark_impl.cc debug/rendering_stats.cc debug/rendering_stats_instrumentation.cc debug/traced_display_item_list.cc debug/traced_value.cc debug/unittest_only_benchmark.cc debug/unittest_only_benchmark_impl.cc)

# input
ADD_LIBRARY(input OBJECT input/browser_controls_offset_manager.cc input/input_handler.cc input/layer_selection_bound.cc input/page_scale_animation.cc input/scroll_elasticity_helper.cc input/scroll_state.cc input/scroll_state_data.cc input/scrollbar_animation_controller.cc input/scrollbar_animation_controller_linear_fade.cc input/scrollbar_animation_controller_thinning.cc input/single_scrollbar_animation_controller_thinning.cc)

# layers
ADD_LIBRARY(layers OBJECT layers/draw_properties.cc layers/heads_up_display_layer.cc layers/heads_up_display_layer_impl.cc layers/layer.cc layers/layer_impl.cc layers/layer_impl_test_properties.cc layers/layer_list_iterator.cc layers/layer_position_constraint.cc layers/layer_sticky_position_constraint.cc layers/layer_utils.cc layers/nine_patch_layer.cc layers/nine_patch_layer_impl.cc layers/painted_scrollbar_layer.cc layers/painted_scrollbar_layer_impl.cc layers/picture_image_layer.cc layers/picture_layer.cc layers/picture_layer_impl.cc layers/render_surface_impl.cc layers/scrollbar_layer_impl_base.cc layers/solid_color_layer.cc layers/solid_color_layer_impl.cc layers/solid_color_scrollbar_layer.cc layers/solid_color_scrollbar_layer_impl.cc layers/surface_layer.cc layers/surface_layer_impl.cc layers/texture_layer.cc layers/texture_layer_impl.cc layers/ui_resource_layer.cc layers/ui_resource_layer_impl.cc layers/video_frame_provider_client_impl.cc layers/video_layer.cc layers/video_layer_impl.cc layers/viewport.cc)

# output
ADD_LIBRARY(output OBJECT output/begin_frame_args.cc output/bsp_tree.cc output/bsp_walk_action.cc output/ca_layer_overlay.cc output/color_lut_cache.cc output/compositor_frame.cc output/compositor_frame_metadata.cc output/compositor_frame_sink.cc output/context_cache_controller.cc output/context_provider.cc output/copy_output_request.cc output/copy_output_result.cc output/direct_renderer.cc output/dynamic_geometry_binding.cc output/filter_operation.cc output/filter_operations.cc output/geometry_binding.cc output/gl_renderer.cc output/gl_renderer_draw_cache.cc output/latency_info_swap_promise.cc output/layer_quad.cc output/managed_memory_policy.cc output/output_surface.cc output/output_surface_frame.cc output/overlay_candidate.cc output/overlay_processor.cc output/overlay_strategy_single_on_top.cc output/overlay_strategy_underlay.cc output/program_binding.cc output/render_surface_filters.cc output/renderer_settings.cc output/shader.cc output/software_output_device.cc output/software_renderer.cc output/static_geometry_binding.cc output/texture_mailbox_deleter.cc)

# playback
ADD_LIBRARY(playback OBJECT playback/clip_display_item.cc playback/clip_path_display_item.cc playback/compositing_display_item.cc playback/decoded_draw_image.cc playback/discardable_image_map.cc playback/display_item_list.cc playback/draw_image.cc playback/raster_source.cc playback/drawing_display_item.cc playback/filter_display_item.cc playback/float_clip_display_item.cc playback/image_hijack_canvas.cc playback/largest_display_item.cc playback/recording_source.cc playback/skip_image_canvas.cc playback/transform_display_item.cc)

# quads
ADD_LIBRARY(quads OBJECT quads/content_draw_quad_base.cc quads/debug_border_draw_quad.cc quads/draw_polygon.cc quads/draw_quad.cc quads/largest_draw_quad.cc quads/picture_draw_quad.cc quads/render_pass.cc quads/render_pass_draw_quad.cc quads/shared_quad_state.cc quads/solid_color_draw_quad.cc quads/stream_video_draw_quad.cc quads/surface_draw_quad.cc quads/texture_draw_quad.cc quads/tile_draw_quad.cc quads/yuv_video_draw_quad.cc)

# raster
ADD_LIBRARY(raster OBJECT raster/bitmap_raster_buffer_provider.cc raster/gpu_raster_buffer_provider.cc raster/one_copy_raster_buffer_provider.cc raster/raster_buffer.cc raster/raster_buffer_provider.cc raster/scoped_gpu_raster.cc raster/single_thread_task_graph_runner.cc raster/staging_buffer_pool.cc raster/synchronous_task_graph_runner.cc raster/task.cc raster/task_graph_work_queue.cc raster/texture_compressor.cc raster/texture_compressor_etc1.cc raster/tile_task.cc raster/zero_copy_raster_buffer_provider.cc)

# resources
ADD_LIBRARY(resources OBJECT resources/memory_history.cc resources/resource_format.cc resources/resource_format_utils.cc resources/resource_pool.cc resources/resource_provider.cc resources/scoped_resource.cc resources/scoped_ui_resource.cc resources/shared_bitmap.cc resources/single_release_callback.cc resources/single_release_callback_impl.cc resources/texture_mailbox.cc resources/transferable_resource.cc resources/ui_resource_bitmap.cc resources/ui_resource_manager.cc resources/ui_resource_request.cc resources/video_resource_updater.cc)
# "video_resource_updater.cc" requires libyuv

# scheduler
ADD_LIBRARY(scheduler OBJECT scheduler/begin_frame_source.cc scheduler/begin_frame_tracker.cc scheduler/compositor_timing_history.cc scheduler/delay_based_time_source.cc scheduler/scheduler.cc scheduler/scheduler_settings.cc scheduler/scheduler_state_machine.cc)

# tiles
ADD_LIBRARY(tiles OBJECT tiles/decoded_image_tracker.cc tiles/eviction_tile_priority_queue.cc tiles/image_controller.cc tiles/mipmap_util.cc tiles/picture_layer_tiling.cc tiles/picture_layer_tiling_set.cc tiles/prioritized_tile.cc tiles/raster_tile_priority_queue.cc tiles/raster_tile_priority_queue_all.cc tiles/raster_tile_priority_queue_required.cc tiles/tile.cc tiles/tile_draw_info.cc tiles/tile_manager.cc tiles/tile_priority.cc tiles/tile_task_manager.cc tiles/tiling_set_eviction_queue.cc tiles/tiling_set_raster_queue_all.cc tiles/tiling_set_raster_queue_required.cc tiles/gpu_image_decode_cache.cc tiles/software_image_decode_cache.cc)

# trees
ADD_LIBRARY(trees OBJECT trees/blocking_task_runner.cc trees/clip_expander.cc trees/clip_node.cc trees/damage_tracker.cc trees/draw_property_utils.cc trees/effect_node.cc trees/element_id.cc trees/latency_info_swap_promise_monitor.cc trees/layer_tree_host.cc trees/layer_tree_host_common.cc trees/layer_tree_host_impl.cc trees/layer_tree_impl.cc trees/layer_tree_settings.cc trees/occlusion.cc trees/occlusion_tracker.cc trees/property_animation_state.cc trees/property_tree.cc trees/property_tree_builder.cc trees/proxy_common.cc trees/proxy_impl.cc trees/proxy_main.cc trees/scroll_node.cc trees/single_thread_proxy.cc trees/swap_promise_manager.cc trees/swap_promise_monitor.cc trees/target_property.cc trees/task_runner_provider.cc trees/transform_node.cc trees/tree_synchronizer.cc)

# surfaces
ADD_LIBRARY(surfaces OBJECT surfaces/direct_compositor_frame_sink.cc surfaces/direct_surface_reference_factory.cc surfaces/display.cc surfaces/display_scheduler.cc surfaces/frame_sink_id.cc surfaces/local_surface_id.cc surfaces/surface.cc surfaces/sequence_surface_reference_factory.cc surfaces/surface_aggregator.cc surfaces/surface_factory.cc surfaces/surface_hittest.cc surfaces/surface_id.cc surfaces/surface_id_allocator.cc surfaces/surface_manager.cc surfaces/surface_resource_holder.cc surfaces/surface_sequence_generator.cc)


# all files refer to "cc/...", so we include "../" here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
#SET(include_dirs ${INCLUDE_DIRS} ${protobuf_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/gpu ${LIBYUV_DIR}/include ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR})
SET(include_dirs ${INCLUDE_DIRS} ${protobuf_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/gpu ${LIBYUV_DIR}/include ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR} ../base)
SET(link_libraries ${LINK_LIBRARIES} ${protobuf_LIBRARIES})

ADD_LIBRARY(cc SHARED $<TARGET_OBJECTS:animation> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:debug> $<TARGET_OBJECTS:input> $<TARGET_OBJECTS:layers> $<TARGET_OBJECTS:output> $<TARGET_OBJECTS:playback> $<TARGET_OBJECTS:quads> $<TARGET_OBJECTS:raster> $<TARGET_OBJECTS:resources> $<TARGET_OBJECTS:scheduler> $<TARGET_OBJECTS:tiles> $<TARGET_OBJECTS:trees> $<TARGET_OBJECTS:surfaces> ${TOOLCHAIN_OPTS_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(cc ${link_libraries})

INSTALL(TARGETS cc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
