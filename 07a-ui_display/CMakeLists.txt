PROJECT(chromium-ui-display CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Display")

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
ADD_DEFINITIONS(-DDISPLAY_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# base
ADD_LIBRARY(base OBJECT display.cc display_change_notifier.cc display_observer.cc screen.cc screen_aura.cc)

# generated (these are generated !)
#ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/accessibility/ax_enums.cc)


# all files refer to "ui/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${SKIA_SRC_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${LINK_LIBRARIES})

ADD_LIBRARY(ui_display SHARED $<TARGET_OBJECTS:base>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_display ${link_libraries})

INSTALL(TARGETS ui_display
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
