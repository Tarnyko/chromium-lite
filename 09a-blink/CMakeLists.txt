PROJECT(chromium-blink C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Blink")

# Location of Base source folder
SET(BASE_DIR "${CMAKE_SOURCE_DIR}/../../../base")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindThreads)
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PACKAGE(Threads)
PKG_CHECK_MODULES(icu REQUIRED icu-i18n icu-uc)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES platform/exported/WebScrollbarImpl.h)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  #PKG_CHECK_MODULES(backend REQUIRED x11)
  SET(BACKEND_SOURCE_FILES platform/exported/WebScrollbarImpl.h)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DBLINK_COMMON_IMPLEMENTATION=1 -DBLINK_PLATFORM_IMPLEMENTATION=1 -DINSIDE_BLINK -DUSE_AURA -DUSING_SYSTEM_ICU)
ADD_DEFINITIONS(-DWTF_IMPLEMENTATION=1)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -Wno-c++0x-compat)
ENDIF()


# SOURCES FILE LIST

# wtf
ADD_LIBRARY(wtf OBJECT wtf/ASCIICType.cpp wtf/AddressSpaceRandomization.cpp wtf/ArrayBuffer.cpp wtf/ArrayBufferBuilder.cpp wtf/ArrayBufferContents.cpp wtf/ArrayBufferView.cpp wtf/ArrayPiece.cpp wtf/Assertions.cpp wtf/BitVector.cpp wtf/CryptographicallyRandomNumber.cpp wtf/CurrentTime.cpp wtf/DataLog.cpp wtf/DateMath.cpp wtf/DynamicAnnotations.cpp wtf/FilePrintStream.cpp wtf/HashTable.cpp wtf/InstanceCounter.cpp wtf/MainThread.cpp wtf/PageAllocator.cpp wtf/PartitionAlloc.cpp wtf/PartitionAllocator.cpp wtf/Partitions.cpp wtf/PrintStream.cpp wtf/RefCountedLeakCounter.cpp wtf/SpinLock wtf/ThreadingPthreads.cpp wtf/TypeTraits.cpp wtf/WTF.cpp wtf/WTFThreadData.cpp wtf/dtoa.cpp wtf/dtoa/bignum-dtoa.cc wtf/dtoa/bignum.cc wtf/dtoa/cached-powers.cc wtf/dtoa/diy-fp.cc wtf/dtoa/double-conversion.cc wtf/dtoa/fast-dtoa.cc wtf/dtoa/fixed-dtoa.cc wtf/dtoa/strtod.cc wtf/text/AtomicString.cpp wtf/text/AtomicStringCF.cpp wtf/text/Base64.cpp wtf/text/CString.cpp wtf/text/StringBuilder.cpp wtf/text/StringConcatenate.cpp wtf/text/StringImpl.cpp wtf/text/StringImplCF.cpp wtf/text/StringStatics.cpp wtf/text/TextCodecICU.cpp wtf/text/TextCodecLatin1.cpp wtf/text/TextCodecReplacement.cpp wtf/text/TextCodecUTF16.cpp wtf/text/TextCodecUTF8.cpp wtf/text/TextCodecUserDefined.cpp wtf/text/TextEncodingRegistry.cpp wtf/text/TextPosition.cpp wtf/text/UTF8.cpp wtf/text/WTFString.cpp wtf/text/icu/CollatorICU.cpp)

# common
ADD_LIBRARY(common OBJECT platform/exported/WebCString.cpp platform/exported/WebString.cpp platform/exported/WebCommon.cpp)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${icu_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${BASE_DIR}/.. ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/wtf ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${icu_LIBRARIES} ${backend_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ldl)

ADD_LIBRARY(blink SHARED $<TARGET_OBJECTS:wtf> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(blink ${link_libraries})

INSTALL(TARGETS blink
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
