PROJECT(chromium-ui-touch-selection CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Touch Selection")

# Location of direct dependencies
SET(UI_RESOURCES_DIR "${CMAKE_SOURCE_DIR}/../resources")

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
INCLUDE(GNUInstallDirs)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DUI_TOUCH_SELECTION_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT longpress_drag_selector.cc touch_handle.cc touch_handle_drawable_aura.cc touch_selection_controller.cc touch_selection_menu_runner.cc)


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${UI_RESOURCES_DIR}/Build ${CMAKE_SOURCE_DIR}/../..)
# TODO : "${UI_RESOURCES_DIR}/Build" is a HACK ! Fix this once we use .pc files

ADD_LIBRARY(ui_touch_selection SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_touch_selection ${link_libraries})

INSTALL(TARGETS ui_touch_selection
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
