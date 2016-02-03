ADD_DEFINITIONS(-m32 -msse2 -mfpmath=sse)
SET(CMAKE_SHARED_LINKER_FLAGS "-m32")

SET(TOOLCHAIN_OPTS_SOURCE_FILES src/opts/SkBitmapFilter_opts_SSE2.cpp src/opts/SkBitmapProcState_opts_SSE2.cpp src/opts/SkBlitRow_opts_SSE2.cpp src/opts/SkBitmapProcState_opts_SSSE3.cpp src/opts/SkOpts_ssse3.cpp src/opts/SkBlitRow_opts_SSE4.cpp src/opts/SkOpts_sse41.cpp src/opts/SkOpts_avx.cpp src/opts/opts_check_x86.cpp)
