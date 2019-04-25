PROJECT(chromium-crypto C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Crypto")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Choose security backend ; default to NSS because we love it ^^
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<NSS/OPENSSL>\" not passed; defaulting to NSS...")
  SET(BACKEND "NSS")
ENDIF(NOT BACKEND)

# Mandatory dependencies
IF(BACKEND STREQUAL "NSS")
  PKG_CHECK_MODULES(backend REQUIRED nss)
  ADD_DEFINITIONS(-DUSE_NSS_CERTS)
ENDIF(BACKEND STREQUAL "NSS")
#
IF(BACKEND STREQUAL "OPENSSL")
  PKG_CHECK_MODULES(backend REQUIRED openssl)
  ADD_DEFINITIONS(-DUSE_OPENSSL)
ENDIF(BACKEND STREQUAL "OPENSSL")

# Common definitions
ADD_DEFINITIONS(-DCRYPTO_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT ec_signature_creator.cc encryptor.cc hkdf.cc hmac.cc p224.cc random.cc rsa_private_key.cc secure_hash_default.cc sha2.cc secure_util.cc)

IF(BACKEND STREQUAL "NSS")
  ADD_LIBRARY(backend OBJECT curve25519-donna.c curve25519_nss.cc ec_private_key_nss.cc ec_signature_creator_nss.cc encryptor_nss.cc hmac_nss.cc nss_key_util.cc nss_util.cc rsa_private_key_nss.cc signature_creator_nss.cc signature_verifier_nss.cc symmetric_key_nss.cc third_party/nss/pk11akey.cc third_party/nss/rsawrapr.c third_party/nss/secsign.cc third_party/nss/sha512.cc)
ENDIF(BACKEND STREQUAL "NSS")

IF(BACKEND STREQUAL "OPENSSL")
  ADD_LIBRARY(backend OBJECT signature_creator.cc signature_verifier.cc symmetric_key.cc)
ENDIF(BACKEND STREQUAL "OPENSSL")


# all files refer to "crypto/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${LINK_LIBRARIES} ${backend_LIBRARIES})

# avoid naming ourselves "crypto", which collides with OpenSSL
ADD_LIBRARY(crcrypto SHARED $<TARGET_OBJECTS:src> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(crcrypto ${link_libraries})

INSTALL(TARGETS crcrypto
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
