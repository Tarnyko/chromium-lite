PROJECT(chromium-sdch CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - SDCH")

# CMake modules
INCLUDE(GNUInstallDirs)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -Wno-deprecated-declarations)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT open-vcdiff/src/addrcache.cc open-vcdiff/src/blockhash.cc open-vcdiff/src/codetable.cc open-vcdiff/src/decodetable.cc open-vcdiff/src/encodetable.cc open-vcdiff/src/headerparser.cc open-vcdiff/src/instruction_map.cc open-vcdiff/src/jsonwriter.cc open-vcdiff/src/logging.cc open-vcdiff/src/varint_bigendian.cc open-vcdiff/src/vcdecoder.cc open-vcdiff/src/vcdiffengine.cc)


# all files refer to "url/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/linux ${CMAKE_SOURCE_DIR}/open-vcdiff/src)

ADD_LIBRARY(sdch SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(sdch ${link_libraries})

INSTALL(TARGETS sdch
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
