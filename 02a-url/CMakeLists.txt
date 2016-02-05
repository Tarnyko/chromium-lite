PROJECT(chromium-url CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Url")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(icuuc REQUIRED icu-uc)

# Common definitions
ADD_DEFINITIONS(-DURL_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(url OBJECT gurl.cc origin.cc scheme_host_port.cc url_canon_etc.cc url_canon_filesystemurl.cc url_canon_fileurl.cc url_canon_host.cc url_canon_icu.cc url_canon_internal.cc url_canon_ip.cc url_canon_mailtourl.cc url_canon_path.cc url_canon_pathurl.cc url_canon_query.cc url_canon_relative.cc url_canon_stdstring.cc url_canon_stdurl.cc url_constants.cc url_parse_file.cc url_util.cc third_party/mozilla/url_parse.cc)


# all files refer to "url/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${icuuc_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${icuuc_LIBRARIES})

ADD_LIBRARY(url_lib SHARED $<TARGET_OBJECTS:url>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(url_lib ${link_libraries})

INSTALL(TARGETS url_lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
