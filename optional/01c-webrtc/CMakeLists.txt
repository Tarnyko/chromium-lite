PROJECT(chromium-webrtc CXX C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - WebRTC Library")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

IF(NOT FULL)
  MESSAGE(STATUS "Option \"-DFULL\" not passed; building audio only...")
  ADD_DEFINITIONS(-DWEBRTC_AUDIO_PROCESSING_ONLY_BUILD)
ELSE()
  ADD_DEFINITIONS(-DFEATURE_ENABLE_SSL)
  IF(NOT SSL)
    MESSAGE(STATUS "Option \"-DSSL=<OPENSSL/BUILTIN>\" not passed; defaulting to OPENSSL...")
    ADD_DEFINITIONS(-DSSL_USE_OPENSSL -DHAVE_OPENSSL_SSL_H)
  ENDIF(NOT SSL)
ENDIF(NOT FULL)

# Check if a Noise Suppression Method has been chosen ; default to FLOAT
IF(NOT NOISESUPPR)
  MESSAGE(STATUS "Option \"-DNOISESUPPR=<FIXED/FLOAT>\" not passed; defaulting to FLOAT...")
  SET(NOISESUPPR "FLOAT")
ENDIF(NOT NOISESUPPR)
MESSAGE(STATUS "Chosen Noise Suppression Method: ${NOISESUPPR}")

# CMake modules
INCLUDE(GNUInstallDirs)

# Optional dependencies and definitions
IF(FULL)
  INCLUDE(FindThreads)
  INCLUDE(FindPkgConfig)

  FIND_PACKAGE(Threads)
  PKG_CHECK_MODULES(openssl REQUIRED openssl)
  PKG_CHECK_MODULES(jsoncpp REQUIRED jsoncpp)
ENDIF(FULL)
#
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

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(base OBJECT common_types.cc base/criticalsection.cc base/checks.cc base/event.cc base/platform_thread.cc base/platform_file.cc base/stringutils.cc base/thread_checker_impl.cc)
 # FOR FULL BUILD
IF(FULL)
  ADD_LIBRARY(base_full OBJECT base/flags.cc base/stringencode.cc base/timeutils.cc base/logging.cc base/event_tracer.cc base/messagehandler.cc base/messagequeue.cc base/socketaddress.cc base/asyncsocket.cc base/physicalsocketserver.cc base/sigslot.cc base/ipaddress.cc base/asyncfile.cc base/asyncresolverinterface.cc base/nethelpers.cc base/signalthread.cc base/thread.cc base/common.cc base/buffer.cc base/stream.cc base/messagedigest.cc base/base64.cc base/md5.cc base/md5digest.cc base/sha1.cc base/sha1digest.cc base/sslidentity.cc base/ssladapter.cc base/sslstreamadapter.cc base/helpers.cc base/json.cc)
  # OPENSSL-SPECIFIC 
  ADD_LIBRARY(base_full-openssl OBJECT base/openssldigest.cc base/opensslidentity.cc base/openssladapter.cc base/opensslstreamadapter.cc)
ENDIF(FULL)

ADD_LIBRARY(syswrap OBJECT system_wrappers/source/aligned_malloc.cc system_wrappers/source/cpu_features.cc system_wrappers/source/critical_section_posix.cc system_wrappers/source/event.cc system_wrappers/source/event_timer_posix.cc system_wrappers/source/file_impl.cc system_wrappers/source/critical_section.cc system_wrappers/source/logging.cc system_wrappers/source/metrics_default.cc system_wrappers/source/rw_lock.cc system_wrappers/source/rw_lock_posix.cc system_wrappers/source/sleep.cc system_wrappers/source/trace_impl.cc system_wrappers/source/trace_posix.cc)

ADD_LIBRARY(audio OBJECT common_audio/audio_converter.cc common_audio/audio_ring_buffer.cc common_audio/audio_util.cc common_audio/blocker.cc common_audio/channel_buffer.cc common_audio/fft4g.c common_audio/fir_filter.cc common_audio/lapped_transform.cc common_audio/real_fourier.cc common_audio/real_fourier_ooura.cc common_audio/ring_buffer.c common_audio/sparse_fir_filter.cc common_audio/wav_file.cc common_audio/wav_header.cc common_audio/window_generator.cc common_audio/resampler/push_resampler.cc common_audio/resampler/push_sinc_resampler.cc common_audio/resampler/resampler.cc common_audio/resampler/sinc_resampler.cc common_audio/resampler/sinusoidal_linear_chirp_source.cc common_audio/signal_processing/auto_corr_to_refl_coef.c common_audio/signal_processing/auto_correlation.c common_audio/signal_processing/complex_fft.c common_audio/signal_processing/complex_bit_reverse.c common_audio/signal_processing/copy_set_operations.c common_audio/signal_processing/cross_correlation.c common_audio/signal_processing/division_operations.c common_audio/signal_processing/dot_product_with_scale.c common_audio/signal_processing/downsample_fast.c common_audio/signal_processing/energy.c common_audio/signal_processing/filter_ar.c common_audio/signal_processing/filter_ar_fast_q12.c common_audio/signal_processing/filter_ma_fast_q12.c common_audio/signal_processing/get_hanning_window.c common_audio/signal_processing/get_scaling_square.c common_audio/signal_processing/ilbc_specific_functions.c common_audio/signal_processing/levinson_durbin.c common_audio/signal_processing/lpc_to_refl_coef.c common_audio/signal_processing/min_max_operations.c common_audio/signal_processing/randomization_functions.c common_audio/signal_processing/real_fft.c common_audio/signal_processing/refl_coef_to_lpc.c common_audio/signal_processing/resample.c common_audio/signal_processing/resample_48khz.c common_audio/signal_processing/resample_by_2.c common_audio/signal_processing/resample_by_2_internal.c common_audio/signal_processing/resample_fractional.c common_audio/signal_processing/spl_init.c common_audio/signal_processing/spl_sqrt.c common_audio/signal_processing/splitting_filter.c common_audio/signal_processing/sqrt_of_one_minus_x_squared.c common_audio/signal_processing/spl_sqrt_floor.c common_audio/signal_processing/vector_scaling_operations.c common_audio/vad/vad.cc common_audio/vad/vad_core.c common_audio/vad/vad_filterbank.c common_audio/vad/vad_gmm.c common_audio/vad/vad_sp.c common_audio/vad/webrtc_vad.c)

ADD_LIBRARY(module_audio OBJECT modules/audio_coding/codecs/isac/main/source/arith_routines.c modules/audio_coding/codecs/isac/main/source/arith_routines_hist.c modules/audio_coding/codecs/isac/main/source/arith_routines_logist.c modules/audio_coding/codecs/isac/main/source/encode_lpc_swb.c modules/audio_coding/codecs/isac/main/source/entropy_coding.c modules/audio_coding/codecs/isac/main/source/filter_functions.c modules/audio_coding/codecs/isac/main/source/filterbanks.c modules/audio_coding/codecs/isac/main/source/filterbank_tables.c modules/audio_coding/codecs/isac/main/source/intialize.c modules/audio_coding/codecs/isac/main/source/lpc_analysis.c modules/audio_coding/codecs/isac/main/source/lpc_gain_swb_tables.c modules/audio_coding/codecs/isac/main/source/lpc_shape_swb12_tables.c modules/audio_coding/codecs/isac/main/source/lpc_shape_swb16_tables.c modules/audio_coding/codecs/isac/main/source/lpc_tables.c modules/audio_coding/codecs/isac/main/source/pitch_estimator.c modules/audio_coding/codecs/isac/main/source/pitch_filter.c modules/audio_coding/codecs/isac/main/source/pitch_gain_tables.c modules/audio_coding/codecs/isac/main/source/pitch_lag_tables.c modules/audio_coding/codecs/isac/main/source/spectrum_ar_model_tables.c modules/audio_processing/audio_buffer.cc modules/audio_processing/audio_processing_impl.cc modules/audio_processing/echo_cancellation_impl.cc modules/audio_processing/echo_control_mobile_impl.cc modules/audio_processing/gain_control_impl.cc modules/audio_processing/high_pass_filter_impl.cc modules/audio_processing/level_estimator_impl.cc modules/audio_processing/noise_suppression_impl.cc modules/audio_processing/rms_level.cc modules/audio_processing/splitting_filter.cc modules/audio_processing/processing_component.cc modules/audio_processing/three_band_filter_bank.cc modules/audio_processing/typing_detection.cc modules/audio_processing/voice_detection_impl.cc modules/audio_processing/aec/aec_core.c modules/audio_processing/aec/aec_rdft.c modules/audio_processing/aec/aec_resampler.c modules/audio_processing/aec/echo_cancellation.c modules/audio_processing/aecm/echo_control_mobile.c modules/audio_processing/aecm/aecm_core.c modules/audio_processing/aecm/aecm_core_c.c modules/audio_processing/agc/agc.cc modules/audio_processing/agc/agc_manager_direct.cc modules/audio_processing/agc/histogram.cc modules/audio_processing/agc/utility.cc modules/audio_processing/agc/legacy/analog_agc.c modules/audio_processing/agc/legacy/digital_agc.c modules/audio_processing/beamformer/array_util.cc modules/audio_processing/beamformer/nonlinear_beamformer.cc modules/audio_processing/beamformer/covariance_matrix_generator.cc modules/audio_processing/intelligibility/intelligibility_enhancer.cc modules/audio_processing/intelligibility/intelligibility_utils.cc modules/audio_processing/agc/legacy/analog_agc.c modules/audio_processing/logging/aec_logging_file_handling.cc modules/audio_processing/transient/click_annotate.cc modules/audio_processing/transient/file_utils.cc modules/audio_processing/transient/moving_moments.cc modules/audio_processing/transient/transient_detector.cc modules/audio_processing/transient/transient_suppressor.cc modules/audio_processing/transient/wpd_node.cc modules/audio_processing/transient/wpd_tree.cc modules/audio_processing/utility/delay_estimator.c modules/audio_processing/utility/delay_estimator_wrapper.c modules/audio_processing/vad/gmm.cc modules/audio_processing/vad/pitch_based_vad.cc modules/audio_processing/vad/pitch_internal.cc modules/audio_processing/vad/pole_zero_filter.cc modules/audio_processing/vad/standalone_vad.cc modules/audio_processing/vad/vad_audio_proc.cc modules/audio_processing/vad/vad_circular_buffer.cc modules/audio_processing/vad/voice_activity_detector.cc ${NOISESUPPR_SOURCE_FILES})

# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)

IF(FULL)
  ADD_LIBRARY(webrtc SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:base_full> $<TARGET_OBJECTS:base_full-openssl> $<TARGET_OBJECTS:syswrap> $<TARGET_OBJECTS:audio> $<TARGET_OBJECTS:module_audio> ${TOOLCHAIN_OPTS_SOURCE_FILES})
  INCLUDE_DIRECTORIES(${include_dirs} ${jsoncpp_INCLUDE_DIRS})
  SET(link_libraries ${openssl_LIBRARIES} ${jsoncpp_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})  
ELSE()
  ADD_LIBRARY(webrtc SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:syswrap> $<TARGET_OBJECTS:audio> $<TARGET_OBJECTS:module_audio> ${TOOLCHAIN_OPTS_SOURCE_FILES})
ENDIF(FULL)

INCLUDE_DIRECTORIES(${include_dirs} common_audio/signal_processing/include)
TARGET_LINK_LIBRARIES(webrtc ${link_libraries})

INSTALL(TARGETS webrtc
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
