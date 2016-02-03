ADD_DEFINITIONS(-march=armv7-a -mthumb)
SET(CMAKE_SHARED_LINKER_FLAGS "-march=armv7-a")

SET(TOOLCHAIN_OPTS_SOURCE_FILES src/opts/SkBitmapProcState_opts_arm.cpp src/opts/SkBlitMask_opts_arm.cpp src/opts/SkBlitRow_opts_arm.cpp)
