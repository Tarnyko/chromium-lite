PROJECT(chromium-crypto CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE, Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Crypto")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
# (we use system OpenSSL/LibreSSL instead of Google's BoringSSL)
PKG_CHECK_MODULES(libssl REQUIRED libssl)

# Common definitions
ADD_DEFINITIONS(-DCRYPTO_IMPLEMENTATION -USE_OPENSSL)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT aead_openssl.cc curve25519_openssl.cc ghash.cc ec_private_key_openssl.cc ec_signature_creator_openssl.cc ec_signature_creator.cc encryptor_openssl.cc encryptor.cc hkdf.cc hmac_openssl.cc openssl_bio_string.cc openssl_util.cc p224.cc random.cc rsa_private_key_openssl.cc rsa_private_key.cc secure_hash_openssl.cc sha2.cc signature_creator_openssl.cc signature_verifier_openssl.cc symmetric_key_openssl.cc)


# all files refer to "crypto/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${libssl_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${libssl_LIBRARIES})

# avoid naming ourselves "crypto", which collides with OpenSSL
ADD_LIBRARY(crcrypto SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(crcrypto ${link_libraries})

INSTALL(TARGETS crcrypto
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
