FIND_PROGRAM(YASM_FOUND yasm)
IF(NOT YASM_FOUND)
  MESSAGE(FATAL_ERROR "\"yasm\" is required !")
ELSE()
  MESSAGE(STATUS "\"yasm\" found...")
ENDIF(NOT YASM_FOUND)


MESSAGE(STATUS "Generating .o files from .asm...")

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/convert_rgb_to_yuv_ssse3.o base/simd/convert_rgb_to_yuv_ssse3.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/convert_yuv_to_rgb_sse.o base/simd/convert_yuv_to_rgb_sse.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/convert_yuva_to_argb_mmx.o base/simd/convert_yuva_to_argb_mmx.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/empty_register_state_mmx.o base/simd/empty_register_state_mmx.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/linear_scale_yuv_to_rgb_mmx.o base/simd/linear_scale_yuv_to_rgb_mmx.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/linear_scale_yuv_to_rgb_sse.o base/simd/linear_scale_yuv_to_rgb_sse.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/scale_yuv_to_rgb_mmx.o base/simd/scale_yuv_to_rgb_mmx.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf32 -m x86 -DARCH_X86_32 -DELF -DCHROMIUM -DEXPORT_SYMBOLS -o ${CMAKE_BINARY_DIR}/scale_yuv_to_rgb_sse.o base/simd/scale_yuv_to_rgb_sse.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


SET(TOOLCHAIN_OPTS_SOURCE_FILES base/simd/convert_rgb_to_yuv_sse2.cc base/simd/convert_rgb_to_yuv_ssse3.cc base/simd/convert_yuv_to_rgb_x86.cc base/simd/filter_yuv_sse2.cc ${CMAKE_BINARY_DIR}/convert_rgb_to_yuv_ssse3.o ${CMAKE_BINARY_DIR}/convert_yuv_to_rgb_sse.o ${CMAKE_BINARY_DIR}/convert_yuva_to_argb_mmx.o ${CMAKE_BINARY_DIR}/empty_register_state_mmx.o ${CMAKE_BINARY_DIR}/linear_scale_yuv_to_rgb_mmx.o ${CMAKE_BINARY_DIR}/linear_scale_yuv_to_rgb_sse.o ${CMAKE_BINARY_DIR}/scale_yuv_to_rgb_mmx.o ${CMAKE_BINARY_DIR}/scale_yuv_to_rgb_sse.o)

