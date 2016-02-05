PROJECT(chromium-crypto C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Crypto")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
# (we use NSS because OpenSSL requires Google BoringSSL)
PKG_CHECK_MODULES(nss REQUIRED nss)

# Common definitions
ADD_DEFINITIONS(-DCRYPTO_IMPLEMENTATION -DUSE_NSS_CERTS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT curve25519-donna.c curve25519_nss.cc ghash.cc ec_private_key_nss.cc ec_signature_creator_nss.cc ec_signature_creator.cc encryptor_nss.cc encryptor.cc hkdf.cc hmac_nss.cc hmac.cc nss_key_util.cc nss_util.cc p224.cc random.cc rsa_private_key_nss.cc rsa_private_key.cc secure_hash_default.cc sha2.cc signature_creator_nss.cc signature_verifier_nss.cc symmetric_key_nss.cc secure_util.cc third_party/nss/pk11akey.cc third_party/nss/rsawrapr.c third_party/nss/secsign.cc third_party/nss/sha512.cc)


# all files refer to "crypto/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${nss_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${nss_LIBRARIES})

# avoid naming ourselves "crypto", which collides with OpenSSL
ADD_LIBRARY(crcrypto SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(crcrypto ${link_libraries})

INSTALL(TARGETS crcrypto
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
