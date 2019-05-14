SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

ADD_DEFINITIONS(-DSK_CPU_SSE_LEVEL=51)   # (= SK_CPU_SSE_LEVEL_AVX)
ADD_DEFINITIONS(-m64 -mfpmath=sse -msse2 -mssse3 -msse4.1 -msse4.2 -mavx -mavx2)
SET(CMAKE_SHARED_LINKER_FLAGS "-m64")

SET(TOOLCHAIN_OPTS_SOURCE_FILES src/opts/SkBitmapProcState_opts_SSE2.cpp src/opts/SkBlitRow_opts_SSE2.cpp src/opts/SkBitmapProcState_opts_SSSE3.cpp src/opts/SkOpts_ssse3.cpp src/opts/SkOpts_sse41.cpp src/opts/SkOpts_sse42.cpp src/opts/SkOpts_avx.cpp src/opts/opts_check_x86.cpp ${SKIA_EXT_DIR}/ext/convolver_SSE2.cc)
