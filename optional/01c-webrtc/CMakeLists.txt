PROJECT(chromium-webrtc CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - WebRTC Library")

# Check if a Noise Suppression Method has been chosen ; default to FLOAT
IF(NOT NOISESUPPR)
  MESSAGE(STATUS "Option \"-DNOISESUPPR=<FIXED/FLOAT>\" not passed; defaulting to FLOAT...")
  SET(NOISESUPPR "FLOAT")
ENDIF(NOT NOISESUPPR)
MESSAGE(STATUS "Chosen Noise Suppression Method: ${NOISESUPPR}")

# CMake moduless
INCLUDE(GNUInstallDirs)

# Optional dependencies and definitions
IF(NOISESUPPR STREQUAL "FIXED")
  ADD_DEFINITIONS(-DWEBRTC_NS_FIXED=1)
  SET(NOISESUPPR_SOURCE_FILES modules/audio_processing/ns/noise_suppression_x.c modules/audio_processing/ns/nsx_core.c modules/audio_processing/ns/nsx_core_c.c)
ENDIF(NOISESUPPR STREQUAL "FIXED")

IF(NOISESUPPR STREQUAL "FLOAT")
  ADD_DEFINITIONS(-DWEBRTC_NS_FLOAT=1)
  SET(NOISESUPPR_SOURCE_FILES modules/audio_processing/ns/noise_suppression.c modules/audio_processing/ns/ns_core.c)
ENDIF(NOISESUPPR STREQUAL "FLOAT")

# Common definitions
ADD_DEFINITIONS(-DWEBRTC_LINUX -DWEBRTC_POSIX)
ADD_DEFINITIONS(-DWEBRTC_AUDIO_PROCESSING_ONLY_BUILD)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(base OBJECT common_types.cc base/criticalsection.cc base/checks.cc base/event.cc base/platform_thread.cc base/platform_file.cc base/stringutils.cc base/thread_checker_impl.cc)

ADD_LIBRARY(syswrap OBJECT system_wrappers/source/aligned_malloc.cc system_wrappers/source/cpu_features.cc system_wrappers/source/critical_section_posix.cc system_wrappers/source/event.cc system_wrappers/source/event_timer_posix.cc system_wrappers/source/file_impl.cc system_wrappers/source/critical_section.cc system_wrappers/source/logging.cc system_wrappers/source/metrics_default.cc system_wrappers/source/rw_lock.cc system_wrappers/source/rw_lock_posix.cc system_wrappers/source/sleep.cc system_wrappers/source/trace_impl.cc system_wrappers/source/trace_posix.cc)

ADD_LIBRARY(audio OBJECT common_audio/audio_converter.cc common_audio/audio_ring_buffer.cc common_audio/audio_util.cc common_audio/blocker.cc common_audio/channel_buffer.cc common_audio/fft4g.c common_audio/fir_filter.cc common_audio/lapped_transform.cc common_audio/real_fourier.cc common_audio/real_fourier_ooura.cc common_audio/ring_buffer.c common_audio/sparse_fir_filter.cc common_audio/wav_file.cc common_audio/wav_header.cc common_audio/window_generator.cc common_audio/resampler/push_resampler.cc common_audio/resampler/push_sinc_resampler.cc common_audio/resampler/resampler.cc common_audio/resampler/sinc_resampler.cc common_audio/resampler/sinusoidal_linear_chirp_source.cc common_audio/signal_processing/auto_corr_to_refl_coef.c common_audio/signal_processing/auto_correlation.c common_audio/signal_processing/copy_set_operations.c common_audio/signal_processing/cross_correlation.c common_audio/signal_processing/division_operations.c common_audio/signal_processing/dot_product_with_scale.c common_audio/signal_processing/downsample_fast.c common_audio/signal_processing/energy.c common_audio/signal_processing/filter_ar.c common_audio/signal_processing/filter_ma_fast_q12.c common_audio/signal_processing/get_hanning_window.c common_audio/signal_processing/get_scaling_square.c common_audio/signal_processing/ilbc_specific_functions.c common_audio/signal_processing/levinson_durbin.c common_audio/signal_processing/lpc_to_refl_coef.c common_audio/signal_processing/min_max_operations.c common_audio/signal_processing/randomization_functions.c common_audio/signal_processing/real_fft.c common_audio/signal_processing/refl_coef_to_lpc.c common_audio/signal_processing/resample.c common_audio/signal_processing/resample_48khz.c common_audio/signal_processing/resample_by_2.c common_audio/signal_processing/resample_by_2_internal.c common_audio/signal_processing/resample_fractional.c common_audio/signal_processing/spl_init.c common_audio/signal_processing/splitting_filter.c common_audio/signal_processing/sqrt_of_one_minus_x_squared.c common_audio/signal_processing/vector_scaling_operations.c common_audio/vad/vad.cc common_audio/vad/vad_core.c common_audio/vad/vad_filterbank.c common_audio/vad/vad_gmm.c common_audio/vad/vad_sp.c common_audio/vad/webrtc_vad.c)

ADD_LIBRARY(module_audio OBJECT modules/audio_coding/codecs/isac/main/source/arith_routines.c modules/audio_coding/codecs/isac/main/source/arith_routines_hist.c modules/audio_coding/codecs/isac/main/source/arith_routines_logist.c modules/audio_coding/codecs/isac/main/source/encode_lpc_swb.c modules/audio_coding/codecs/isac/main/source/entropy_coding.c modules/audio_coding/codecs/isac/main/source/filter_functions.c modules/audio_coding/codecs/isac/main/source/filterbanks.c modules/audio_coding/codecs/isac/main/source/filterbank_tables.c modules/audio_coding/codecs/isac/main/source/intialize.c modules/audio_coding/codecs/isac/main/source/lpc_analysis.c modules/audio_coding/codecs/isac/main/source/lpc_gain_swb_tables.c modules/audio_coding/codecs/isac/main/source/lpc_shape_swb12_tables.c modules/audio_coding/codecs/isac/main/source/lpc_shape_swb16_tables.c modules/audio_coding/codecs/isac/main/source/lpc_tables.c modules/audio_coding/codecs/isac/main/source/pitch_estimator.c modules/audio_coding/codecs/isac/main/source/pitch_filter.c modules/audio_coding/codecs/isac/main/source/pitch_gain_tables.c modules/audio_coding/codecs/isac/main/source/pitch_lag_tables.c modules/audio_coding/codecs/isac/main/source/spectrum_ar_model_tables.c modules/audio_processing/audio_buffer.cc modules/audio_processing/audio_processing_impl.cc modules/audio_processing/echo_cancellation_impl.cc modules/audio_processing/echo_control_mobile_impl.cc modules/audio_processing/gain_control_impl.cc modules/audio_processing/high_pass_filter_impl.cc modules/audio_processing/level_estimator_impl.cc modules/audio_processing/noise_suppression_impl.cc modules/audio_processing/rms_level.cc modules/audio_processing/splitting_filter.cc modules/audio_processing/processing_component.cc modules/audio_processing/three_band_filter_bank.cc modules/audio_processing/typing_detection.cc modules/audio_processing/voice_detection_impl.cc ${NOISESUPPR_SOURCE_FILES})

# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)

ADD_LIBRARY(webrtc SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:syswrap> $<TARGET_OBJECTS:audio> $<TARGET_OBJECTS:module_audio>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(webrtc ${link_libraries})

INSTALL(TARGETS webrtc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
