PROJECT(chromium-freetype C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - FreeType")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(zlib REQUIRED zlib)

# Common definitions
ADD_DEFINITIONS(-DFT_CONFIG_OPTION_SYSTEM_ZLIB -DFT2_BUILD_LIBRARY -DFT_CONFIG_CONFIG_H=<ftconfig.h> -DFT_CONFIG_MODULES_H=<ftmodule.h>)


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT src/src/base/ftsystem.c src/src/base/ftinit.c src/src/base/ftdebug.c src/src/base/ftbase.c src/src/base/ftbbox.c src/src/base/ftglyph.c src/src/base/ftbdf.c src/src/base/ftbitmap.c src/src/base/ftcid.c src/src/base/ftfstype.c src/src/base/ftgasp.c src/src/base/ftgxval.c src/src/base/ftlcdfil.c src/src/base/ftmm.c src/src/base/ftpfr.c src/src/base/ftstroke.c src/src/base/ftsynth.c src/src/base/fttype1.c src/src/base/ftwinfnt.c src/src/base/ftxf86.c src/src/bdf/bdf.c src/src/cff/cff.c src/src/cid/type1cid.c src/src/pcf/pcf.c src/src/pfr/pfr.c src/src/sfnt/sfnt.c src/src/truetype/truetype.c src/src/type1/type1.c src/src/type42/type42.c src/src/winfonts/winfnt.c src/src/psaux/psaux.c src/src/psnames/psnames.c src/src/pshinter/pshinter.c src/src/raster/raster.c src/src/smooth/smooth.c src/src/autofit/autofit.c src/src/gzip/ftgzip.c src/src/lzw/ftlzw.c)


# all files refer to "base/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${zlib_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/include)
SET(link_libraries ${LINK_LIBRARIES} ${zlib_LIBRARIES})

ADD_LIBRARY(crfreetype SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(crfreetype ${link_libraries})

INSTALL(TARGETS crfreetype
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
