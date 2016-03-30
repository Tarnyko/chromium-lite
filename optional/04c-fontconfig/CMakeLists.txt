PROJECT(chromium-fontconfig C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Fontconfig")

# Location of direct dependencies
SET(FREETYPE_DIR "${CMAKE_SOURCE_DIR}/../freetype2")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(zlib REQUIRED zlib)
PKG_CHECK_MODULES(libxml2 REQUIRED libxml-2.0)

# Common definitions
ADD_DEFINITIONS(-DHAVE_CONFIG_H -DFC_CACHEDIR="/var/cache/fontconfig" -DFONTCONFIG_PATH="/etc/fonts")


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT chromium/empty.cc src/src/fcatomic.c src/src/fcblanks.c src/src/fccache.c src/src/fccfg.c src/src/fccharset.c src/src/fccompat.c src/src/fcdbg.c src/src/fcdefault.c src/src/fcdir.c src/src/fcformat.c src/src/fcfreetype.c src/src/fcfs.c src/src/fchash.c src/src/fcinit.c src/src/fclang.c src/src/fclist.c src/src/fcmatch.c src/src/fcmatrix.c src/src/fcname.c src/src/fcobjs.c src/src/fcpat.c src/src/fcserialize.c src/src/fcstat.c src/src/fcstr.c src/src/fcxml.c src/src/ftglue.c)


# all files refer to "base/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${zlib_INCLUDE_DIRS} ${libxml2_INCLUDE_DIRS} ${FREETYPE_DIR}/src/include ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/src)
SET(link_libraries ${LINK_LIBRARIES} ${zlib_LIBRARIES} ${libxml2_LIBRARIES})

ADD_LIBRARY(crfontconfig SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(crfontconfig ${link_libraries})

INSTALL(TARGETS crfontconfig
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
