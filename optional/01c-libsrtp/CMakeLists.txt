PROJECT(chromium-libsrtp C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - SRTP Library")

# CMake modules
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DHAVE_STDINT_H -DHAVE_STDLIB_H -DHAVE_UINT64_T -DHAVE_CONFIG_H)
ADD_DEFINITIONS(-DCPU_CISC -DFORCE_64BIT_ALIGN)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT srtp/srtp/ekt.c srtp/srtp/srtp.c srtp/crypto/cipher/aes.c srtp/crypto/cipher/aes_cbc.c srtp/crypto/cipher/aes_icm.c srtp/crypto/cipher/cipher.c srtp/crypto/cipher/null_cipher.c srtp/crypto/hash/auth.c srtp/crypto/hash/hmac.c srtp/crypto/hash/null_auth.c srtp/crypto/hash/sha1.c srtp/crypto/kernel/alloc.c srtp/crypto/kernel/crypto_kernel.c srtp/crypto/kernel/err.c srtp/crypto/kernel/key.c srtp/crypto/math/datatypes.c srtp/crypto/math/gf2_8.c srtp/crypto/math/stat.c srtp/crypto/replay/rdb.c srtp/crypto/replay/rdbx.c srtp/crypto/replay/ut_sim.c srtp/crypto/rng/ctr_prng.c srtp/crypto/rng/rand_source.c)


SET(include_dirs ${CMAKE_SOURCE_DIR}/config ${CMAKE_SOURCE_DIR}/srtp/include ${CMAKE_SOURCE_DIR}/srtp/crypto/include)

ADD_LIBRARY(srtp SHARED $<TARGET_OBJECTS:src>)

INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(srtp ${link_libraries})

INSTALL(TARGETS srtp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
