SET(QCMS_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/qcms")

ADD_DEFINITIONS(-DSSE2_ENABLE)

SET(TOOLCHAIN_OPTS_SOURCE_FILES ${QCMS_DIR}/src/transform-sse2.c)

# Generate "platform/heap/asm/SaveRegisters_x86.asm" !

