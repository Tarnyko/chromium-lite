ADD_DEFINITIONS("-Wno-uninitialized")

SET(CMAKE_ASM_FLAGS "${CXXFLAGS} -x assembler-with-cpp")
SET(TOOLCHAIN_OPTS_SOURCE_FILES platform/heap/asm/SaveRegisters_arm.S)
