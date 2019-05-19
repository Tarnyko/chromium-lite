PROJECT(chromium-ui-display CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Display")

# re2 source path, needed for "re2/stringpiece.h"
SET(RE2_SRC_DIR "${CMAKE_SOURCE_DIR}/../../third_party/re2")

# Location of Skia source folders
SET(SKIA_SRC_DIR "${CMAKE_SOURCE_DIR}/../../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DDISPLAY_IMPLEMENTATION -DUSE_AURA -D__STDC_FORMAT_MACROS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# base
ADD_LIBRARY(base OBJECT display.cc display_change_notifier.cc display_observer.cc display_switches.cc fake_display_delegate.cc fake_display_snapshot.cc screen.cc screen_aura.cc)

# types
ADD_LIBRARY(types OBJECT types/display_mode.cc types/display_snapshot.cc types/native_display_delegate.cc)

# util
ADD_LIBRARY(util OBJECT util/display_util.cc)


# all files refer to "ui/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${RE2_SRC_DIR}/src ${SKIA_SRC_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${LINK_LIBRARIES})

ADD_LIBRARY(ui_display SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:types> $<TARGET_OBJECTS:util>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_display ${link_libraries})

INSTALL(TARGETS ui_display
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
