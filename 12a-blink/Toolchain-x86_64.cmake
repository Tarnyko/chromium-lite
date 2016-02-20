SET(QCMS_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/qcms")

ADD_DEFINITIONS(-DSSE2_ENABLE)


# GENERATING ASSEMBLY CODE

MESSAGE(STATUS "Generating .o from .asm...")

EXECUTE_PROCESS(COMMAND mkdir -p platform/heap/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND yasm -I.. -felf64 -m amd64 -DPIC -DX64POSIX=1 -DPREFIX -o ${CMAKE_BINARY_DIR}/platform/heap/SaveRegisters_x86.o platform/heap/asm/SaveRegisters_x86.asm WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


SET(TOOLCHAIN_OPTS_SOURCE_FILES ${QCMS_DIR}/src/transform-sse2.c ${CMAKE_BINARY_DIR}/platform/heap/SaveRegisters_x86.o)
