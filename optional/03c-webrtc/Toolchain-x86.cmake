#ADD_DEFINITIONS(-DWEBRTC_ARCH_X86_FAMILY)

SET(TOOLCHAIN_OPTS_SOURCE_FILES modules/audio_processing/aec/aec_core_sse2.c common_audio/resampler/sinc_resampler_sse.cc common_audio/fir_filter_sse.cc modules/audio_processing/aec/aec_rdft_sse2.c)
