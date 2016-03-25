PROJECT(chromium-ui-compositor CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Compositor")

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

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DCOMPOSITOR_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
# src
ADD_LIBRARY(src OBJECT callback_layer_animation_observer.cc canvas_painter.cc clip_recorder.cc closure_animation_observer.cc compositing_recorder.cc compositor.cc compositor_switches.cc compositor_vsync_manager.cc debug_utils.cc dip_util.cc float_animation_curve_adapter.cc layer.cc layer_animation_element.cc layer_animation_observer.cc layer_animation_sequence.cc layer_animator.cc layer_animator_collection.cc layer_owner.cc layer_tree_owner.cc paint_cache.cc paint_context.cc paint_recorder.cc reflector.cc scoped_animation_duration_scale_mode.cc scoped_layer_animation_settings.cc transform_animation_curve_adapter.cc transform_recorder.cc)


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../.. ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core)

ADD_LIBRARY(ui_compositor SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_compositor ${LINK_LIBRARIES})

INSTALL(TARGETS ui_compositor
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
