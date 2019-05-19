PROJECT(chromium-media C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Media")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Location of build tools dir (required for generating "media_features.h")
SET(BUILD_TOOLS_DIR "${CMAKE_SOURCE_DIR}/../build")

# Location of Mojo dependencies, required for .mojom file interpretation
SET(MOJO_SRC_DIR "${CMAKE_SOURCE_DIR}/../mojo")
SET(UI_GFX_DIR "${CMAKE_SOURCE_DIR}/../ui/gfx")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../gpu")

# Location of libwebm/libyuv source folders
SET(LIBWEBM_DIR "${CMAKE_SOURCE_DIR}/../third_party/libwebm")
SET(LIBYUV_DIR "${CMAKE_SOURCE_DIR}/../third_party/libyuv")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(CheckLibraryExists)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(alsa REQUIRED alsa)
# TODO : use crypto dependencies instead of doing this !!
PKG_CHECK_MODULES(deps REQUIRED nss nspr)

# libjpeg-turbo (for libyuv)
IF(EXISTS "/usr/include/jpeglib.h")
  SET(HAVE_LIBJPEG_TURBO_H "1")
ENDIF()
CHECK_LIBRARY_EXISTS(jpeg jpeg_start_decompress "" HAVE_LIBJPEG_TURBO_SO)
IF(HAVE_LIBJPEG_TURBO_H)
  IF(HAVE_LIBJPEG_TURBO_SO)
    SET(HAVE_LIBJPEG_TURBO "1")
  ENDIF(HAVE_LIBJPEG_TURBO_SO)
ENDIF(HAVE_LIBJPEG_TURBO_H)
IF(NOT HAVE_LIBJPEG_TURBO)
  MESSAGE(FATAL_ERROR "\"jpeglib.h\" or \"libjpeg.so\" missing.
    Please install the \"libjpeg-turbo-devel\" or \"libjpeg62-turbo-dev\" package !")
ENDIF(NOT HAVE_LIBJPEG_TURBO)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  #SET(BACKEND_SOURCE_FILES ozone/media_ozone_platform.cc)
  # TODO : this does not work, discover why
  ADD_DEFINITIONS(-DUSE_OZONE -DDISABLE_USER_INPUT_MONITOR)
  # Ozone backend requires generated "ui_ozone" headers
  SET(UI_OZONE_DIR "${CMAKE_SOURCE_DIR}/../ui/ozone")
  SET(INCLUDE_DIRS "${UI_OZONE_DIR}/Build")
  # TODO : "${UI_OZONE_DIR}/Build" here is a HACK ! remove this when we use .pc files
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xdamage xext xfixes xtst)
  SET(BACKEND_SOURCE_FILES base/user_input_monitor_linux.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DMEDIA_IMPLEMENTATION -DMIDI_IMPLEMENTATION -DUSE_ALSA -DMEDIA_DISABLE_FFMPEG -DMEDIA_DISABLE_LIBVPX -DUSE_AURA -D_FILE_OFFSET_BITS=64 -D__STDC_FORMAT_MACROS)
# for libyuv
ADD_DEFINITIONS(-DHAVE_JPEG)


IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION MEDIA_FEATURES.H (NEEDED BY "eme_constants.h")

MESSAGE(STATUS "Generating features.h...")

EXECUTE_PROCESS(COMMAND mkdir -p media WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND sh -c "echo --flags ENABLE_AC3_EAC3_AUDIO_DEMUXING=0 ENABLE_CBCS_ENCRYPTION_SCHEME=0 ENABLE_HEVC_DEMUXING=0 ENABLE_HLS_SAMPLE_AES=0 ENABLE_MSE_MPEG2TS_STREAM_PARSER=0 ENABLE_MEDIA_REMOTING=0 ENABLE_WEBRTC=0 USE_PROPRIETARY_CODECS=0 > buildflag_header.rsp" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND python ${BUILD_TOOLS_DIR}/write_buildflag_header.py --output media/media_features.h --rulename //media:media_features --gen-dir ${CMAKE_BINARY_DIR} --definitions ${CMAKE_BINARY_DIR}/buildflag_header.rsp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .TMPL -> .ZIP (bytecode)

MESSAGE(STATUS "Generating .zip files from .tmpl...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs precompile -o ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p media/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate capture/mojo/image_capture.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate capture/mojo/image_capture.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate midi/midi_service.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate midi/midi_service.mojom -d . -I. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate mojo/interfaces/media_types.mojom -d . -I.. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate mojo/interfaces/media_types.mojom -d . -I.. -o ${CMAKE_BINARY_DIR}/media --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# audio_alsa
ADD_LIBRARY(audio_alsa OBJECT audio/alsa/alsa_input.cc audio/alsa/alsa_output.cc audio/alsa/alsa_util.cc audio/alsa/alsa_wrapper.cc audio/alsa/audio_manager_alsa.cc)

# audio_pulse
#ADD_LIBRARY(audio_pulse OBJECT audio/pulse/pulse_input.cc audio/pulse/pulse_output.cc audio/pulse/pulse_util.cc audio/pulse/audio_manager_pulse.cc)

# audio
ADD_LIBRARY(audio OBJECT audio/audio_device_description.cc audio/audio_device_name.cc audio/audio_device_thread.cc audio/audio_input_controller.cc audio/audio_input_device.cc audio/audio_input_ipc.cc audio/audio_manager.cc audio/audio_manager_base.cc audio/audio_output_controller.cc audio/audio_output_device.cc audio/audio_output_dispatcher.cc audio/audio_output_dispatcher_impl.cc audio/audio_output_ipc.cc audio/audio_output_proxy.cc audio/audio_output_resampler.cc audio/audio_output_stream_sink.cc audio/audio_power_monitor.cc audio/audio_streams_tracker.cc audio/clockless_audio_sink.cc audio/fake_audio_input_stream.cc audio/fake_audio_log_factory.cc audio/fake_audio_manager.cc audio/fake_audio_output_stream.cc audio/virtual_audio_input_stream.cc audio/virtual_audio_output_stream.cc audio/fake_audio_worker.cc audio/null_audio_sink.cc audio/sample_rates.cc audio/scoped_task_runner_observer.cc audio/simple_sources.cc audio/sounds/audio_stream_handler.cc audio/sounds/sounds_manager.cc audio/sounds/wav_audio_handler.cc audio/linux/audio_manager_linux.cc)

# base
ADD_LIBRARY(base OBJECT base/audio_block_fifo.cc base/audio_buffer.cc base/audio_buffer_converter.cc base/audio_buffer_queue.cc base/audio_bus.cc base/audio_codecs.cc base/audio_converter.cc base/audio_decoder.cc base/audio_decoder_config.cc base/audio_discard_helper.cc base/audio_fifo.cc base/audio_hash.cc base/audio_latency.cc base/audio_parameters.cc base/audio_point.cc base/audio_pull_fifo.cc base/audio_renderer.cc base/audio_renderer_mixer.cc base/audio_renderer_mixer_input.cc base/audio_shifter.cc base/audio_timestamp_helper.cc base/bit_reader.cc base/bit_reader_core.cc base/bitstream_buffer.cc base/byte_queue.cc base/cdm_callback_promise.cc base/cdm_context.cc base/cdm_factory.cc base/cdm_initialized_promise.cc base/cdm_key_information.cc base/cdm_promise.cc base/cdm_promise_adapter.cc base/channel_layout.cc base/channel_mixer.cc base/channel_mixing_matrix.cc base/content_decryption_module.cc base/data_buffer.cc base/data_source.cc base/decode_status.cc base/decoder_buffer.cc base/decoder_buffer_queue.cc base/decrypt_config.cc base/decryptor.cc base/demuxer.cc base/demuxer_stream.cc base/djb2.cc base/encryption_scheme.cc base/fake_audio_renderer_sink.cc base/hdr_metadata.cc base/key_system_names.cc base/key_system_properties.cc base/key_systems.cc base/keyboard_event_counter.cc base/localized_strings.cc base/loopback_audio_converter.cc base/media.cc  base/media_client.cc base/media_log.cc base/media_permission.cc base/media_resource.cc base/media_switches.cc base/media_track.cc base/media_tracks.cc base/media_util.cc base/mime_util.cc base/mime_util_internal.cc base/moving_average.cc base/multi_channel_resampler.cc base/null_video_sink.cc base/output_device_info.cc base/pipeline_impl.cc base/pipeline_metadata.cc base/player_tracker.cc base/ranges.cc base/renderer.cc base/renderer_factory.cc base/sample_format.cc base/seekable_buffer.cc base/serial_runner.cc base/simd/convert_rgb_to_yuv_c.cc base/simd/convert_yuv_to_rgb_c.cc base/simd/filter_yuv_c.cc base/sinc_resampler.cc base/stream_parser.cc base/stream_parser_buffer.cc base/text_cue.cc base/text_ranges.cc base/text_renderer.cc base/text_track_config.cc base/time_delta_interpolator.cc base/user_input_monitor.cc base/vector_math.cc base/video_codecs.cc base/video_decoder.cc base/video_decoder_config.cc base/video_frame.cc base/video_frame_metadata.cc base/video_frame_pool.cc base/video_renderer.cc base/video_types.cc base/video_util.cc base/wall_clock_time_source.cc base/yuv_convert.cc base/ipc/media_param_traits.cc)

# capture
ADD_LIBRARY(capture OBJECT capture/content/animated_content_sampler.cc capture/content/capture_resolution_chooser.cc capture/content/screen_capture_device_core.cc capture/content/thread_safe_capture_oracle.cc capture/content/smooth_event_sampler.cc capture/content/video_capture_oracle.cc capture/ipc/capture_param_traits.cc capture/video/blob_utils.cc capture/video/fake_video_capture_device.cc capture/video/fake_video_capture_device_factory.cc capture/video/file_video_capture_device.cc capture/video/file_video_capture_device_factory.cc capture/video/linux/v4l2_capture_delegate.cc capture/video/linux/video_capture_device_factory_linux.cc capture/video/linux/video_capture_device_linux.cc capture/video/video_capture_device.cc capture/video/video_capture_device_descriptor.cc capture/video/video_capture_device_factory.cc capture/video_capture_types.cc capture/video_capturer_source.cc)

# cdm
ADD_LIBRARY(cdm OBJECT cdm/aes_decryptor.cc cdm/cdm_adapter.cc cdm/cdm_allocator.cc cdm/cdm_helpers.cc cdm/default_cdm_factory.cc cdm/json_web_key.cc cdm/player_tracker_impl.cc cdm/simple_cdm_allocator.cc cdm/simple_cdm_buffer.cc cdm/supported_cdm_versions.cc)

# filters
ADD_LIBRARY(filters OBJECT filters/audio_clock.cc filters/audio_renderer_algorithm.cc filters/audio_timestamp_validator.cc filters/chunk_demuxer.cc filters/decoder_selector.cc filters/decoder_stream.cc filters/decoder_stream_traits.cc filters/decrypting_audio_decoder.cc filters/decrypting_demuxer_stream.cc filters/decrypting_video_decoder.cc filters/file_data_source.cc filters/frame_processor.cc filters/gpu_video_decoder.cc filters/h264_bit_reader.cc filters/h264_parser.cc filters/ivf_parser.cc filters/jpeg_parser.cc filters/opus_constants.cc filters/source_buffer_platform.cc filters/source_buffer_range.cc filters/source_buffer_state.cc filters/source_buffer_stream.cc filters/stream_parser_factory.cc filters/video_cadence_estimator.cc filters/video_renderer_algorithm.cc filters/vp8_bool_decoder.cc filters/vp8_parser.cc filters/vp9_parser.cc filters/vp9_compressed_header_parser.cc filters/vp9_uncompressed_header_parser.cc filters/vp9_bool_decoder.cc filters/vp9_raw_bits_reader.cc filters/wsola_internals.cc)
# "filters/audio_file_reader.cc" and "filters/in_memory_url_protocol.cc" 
# are disabled because they pull FFmpeg

# formats
ADD_LIBRARY(formats OBJECT formats/common/offset_byte_queue.cc formats/webm/webm_audio_client.cc formats/webm/webm_cluster_parser.cc formats/webm/webm_colour_parser.cc formats/webm/webm_constants.cc formats/webm/webm_content_encodings.cc formats/webm/webm_content_encodings_client.cc formats/webm/webm_crypto_helpers.cc formats/webm/webm_info_parser.cc formats/webm/webm_parser.cc formats/webm/webm_stream_parser.cc formats/webm/webm_tracks_parser.cc formats/webm/webm_video_client.cc formats/webm/webm_webvtt_parser.cc muxers/webm_muxer.cc)

# midi
ADD_LIBRARY(midi OBJECT midi/message_util.cc midi/midi_manager.cc midi/midi_message_queue.cc midi/midi_port_info.cc midi/midi_scheduler.cc midi/midi_switches.cc)
# "midi/midi_manager_alsa.cc" requires "udev" and "-DUSE_UDEV"

# renderers
ADD_LIBRARY(renderers OBJECT renderers/audio_renderer_impl.cc renderers/default_renderer_factory.cc renderers/renderer_impl.cc renderers/skcanvas_video_renderer.cc renderers/video_renderer_impl.cc)

# video
ADD_LIBRARY(video OBJECT video/fake_video_encode_accelerator.cc video/gpu_memory_buffer_video_frame_pool.cc video/h264_poc.cc video/jpeg_decode_accelerator.cc video/picture.cc video/video_decode_accelerator.cc video/video_encode_accelerator.cc)

# libwebm
ADD_LIBRARY(libwebm OBJECT ${LIBWEBM_DIR}/source/mkvmuxer/mkvmuxer.cc ${LIBWEBM_DIR}/source/mkvmuxer/mkvmuxerutil.cc ${LIBWEBM_DIR}/source/mkvmuxer/mkvwriter.cc)

# libyuv
ADD_LIBRARY(libyuv OBJECT ${LIBYUV_DIR}/source/compare.cc ${LIBYUV_DIR}/source/compare_common.cc ${LIBYUV_DIR}/source/compare_gcc.cc ${LIBYUV_DIR}/source/convert.cc ${LIBYUV_DIR}/source/convert_argb.cc ${LIBYUV_DIR}/source/convert_from.cc ${LIBYUV_DIR}/source/convert_from_argb.cc ${LIBYUV_DIR}/source/convert_jpeg.cc ${LIBYUV_DIR}/source/convert_to_argb.cc ${LIBYUV_DIR}/source/convert_to_i420.cc ${LIBYUV_DIR}/source/cpu_id.cc ${LIBYUV_DIR}/source/cpu_id.cc ${LIBYUV_DIR}/source/mjpeg_decoder.cc ${LIBYUV_DIR}/source/mjpeg_validate.cc ${LIBYUV_DIR}/source/planar_functions.cc ${LIBYUV_DIR}/source/rotate.cc ${LIBYUV_DIR}/source/rotate_any.cc ${LIBYUV_DIR}/source/rotate_argb.cc ${LIBYUV_DIR}/source/rotate_common.cc ${LIBYUV_DIR}/source/rotate_gcc.cc ${LIBYUV_DIR}/source/row_any.cc ${LIBYUV_DIR}/source/row_common.cc ${LIBYUV_DIR}/source/row_gcc.cc ${LIBYUV_DIR}/source/scale.cc ${LIBYUV_DIR}/source/scale_any.cc ${LIBYUV_DIR}/source/scale_argb.cc ${LIBYUV_DIR}/source/scale_common.cc ${LIBYUV_DIR}/source/scale_gcc.cc ${LIBYUV_DIR}/source/video_common.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/media/capture/mojo/image_capture.mojom.cc ${CMAKE_BINARY_DIR}/media/capture/mojo/image_capture.mojom-shared.cc ${CMAKE_BINARY_DIR}/media/midi/midi_service.mojom.cc ${CMAKE_BINARY_DIR}/media/midi/midi_service.mojom-shared.cc ${CMAKE_BINARY_DIR}/media/mojo/interfaces/media_types.mojom.cc ${CMAKE_BINARY_DIR}/media/mojo/interfaces/media_types.mojom-shared.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# some files refer to "media/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
# we include UI_GFX_DIR for "geometry.mojom.h"
SET(include_dirs ${INCLUDE_DIRS} ${alsa_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${deps_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${LIBWEBM_DIR}/source ${LIBYUV_DIR}/include ${UI_GFX_DIR}/Build/ui ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/media ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${LINK_LIBRARIES} ${alsa_LIBRARIES} ${backend_LIBRARIES} ${deps_LIBRARIES} -ljpeg)

ADD_LIBRARY(media SHARED $<TARGET_OBJECTS:audio_alsa> $<TARGET_OBJECTS:audio> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:capture> $<TARGET_OBJECTS:cdm> $<TARGET_OBJECTS:filters> $<TARGET_OBJECTS:formats> $<TARGET_OBJECTS:midi> $<TARGET_OBJECTS:renderers> $<TARGET_OBJECTS:video> $<TARGET_OBJECTS:libwebm> $<TARGET_OBJECTS:libyuv>  $<TARGET_OBJECTS:generated> $<TARGET_OBJECTS:backend> ${TOOLCHAIN_OPTS_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(media ${link_libraries})

INSTALL(TARGETS media
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
