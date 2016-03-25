PROJECT(chromium-ui-aura CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Aura")

# Location of auxiliary folders
SET(WM_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../wm")
SET(NATIVE_THEME_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../native_theme")
SET(AURA_EXTRA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../aura_extra")

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
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
# Useless, but this variable cannot be empty
  SET(BACKEND_SOURCE_FILES window.h)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xrandr xi)
  SET(BACKEND_SOURCE_FILES window_tree_host_x11.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DAURA_IMPLEMENTATION -DAURA_EXTRA_IMPLEMENTATION -DWM_IMPLEMENTATION -DNATIVE_THEME_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# wm
ADD_LIBRARY(wm OBJECT ${WM_SOURCE_DIR}/core/accelerator_filter.cc ${WM_SOURCE_DIR}/core/base_focus_rules.cc ${WM_SOURCE_DIR}/core/capture_controller.cc ${WM_SOURCE_DIR}/core/compound_event_filter.cc ${WM_SOURCE_DIR}/core/coordinate_conversion.cc ${WM_SOURCE_DIR}/core/cursor_manager.cc ${WM_SOURCE_DIR}/core/default_activation_client.cc ${WM_SOURCE_DIR}/core/default_screen_position_client.cc ${WM_SOURCE_DIR}/core/easy_resize_window_targeter.cc ${WM_SOURCE_DIR}/core/focus_controller.cc ${WM_SOURCE_DIR}/core/image_grid.cc ${WM_SOURCE_DIR}/core/masked_window_targeter.cc ${WM_SOURCE_DIR}/core/nested_accelerator_controller.cc ${WM_SOURCE_DIR}/core/nested_accelerator_dispatcher.cc ${WM_SOURCE_DIR}/core/nested_accelerator_dispatcher_linux.cc ${WM_SOURCE_DIR}/core/shadow.cc ${WM_SOURCE_DIR}/core/shadow_controller.cc ${WM_SOURCE_DIR}/core/shadow_types.cc ${WM_SOURCE_DIR}/core/transient_window_controller.cc ${WM_SOURCE_DIR}/core/transient_window_manager.cc ${WM_SOURCE_DIR}/core/transient_window_stacking_client.cc ${WM_SOURCE_DIR}/core/visibility_controller.cc ${WM_SOURCE_DIR}/core/window_animations.cc ${WM_SOURCE_DIR}/core/window_modality_controller.cc ${WM_SOURCE_DIR}/core/window_util.cc ${WM_SOURCE_DIR}/core/wm_core_switches.cc ${WM_SOURCE_DIR}/core/wm_state.cc ${WM_SOURCE_DIR}/public/activation_change_observer.cc ${WM_SOURCE_DIR}/public/activation_client.cc ${WM_SOURCE_DIR}/public/activation_delegate.cc ${WM_SOURCE_DIR}/public/animation_host.cc ${WM_SOURCE_DIR}/public/dispatcher_client.cc ${WM_SOURCE_DIR}/public/drag_drop_client.cc ${WM_SOURCE_DIR}/public/drag_drop_delegate.cc ${WM_SOURCE_DIR}/public/scoped_drag_drop_disabler.cc ${WM_SOURCE_DIR}/public/scoped_tooltip_disabler.cc ${WM_SOURCE_DIR}/public/tooltip_client.cc ${WM_SOURCE_DIR}/public/transient_window_client.cc ${WM_SOURCE_DIR}/public/window_move_client.cc)

# native_theme
ADD_LIBRARY(native_theme OBJECT ${NATIVE_THEME_SOURCE_DIR}/common_theme.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme_base.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme_aura.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme_dark_aura.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme_observer.cc ${NATIVE_THEME_SOURCE_DIR}/native_theme_switches.cc)

# extra
ADD_LIBRARY(extra OBJECT ${AURA_EXTRA_SOURCE_DIR}/image_window_delegate.cc)

# client
ADD_LIBRARY(client OBJECT client/aura_constants.cc client/capture_client.cc client/cursor_client.cc client/cursor_client_observer.cc client/default_capture_client.cc client/event_client.cc client/focus_change_observer.cc client/focus_client.cc client/screen_position_client.cc client/visibility_client.cc client/window_stacking_client.cc client/window_tree_client.cc)

# misc
ADD_LIBRARY(misc OBJECT env.cc input_state_lookup.cc layout_manager.cc scoped_window_targeter.cc window.cc window_event_dispatcher.cc window_observer.cc window_targeter.cc window_tracker.cc window_tree_host.cc window_tree_host_platform.cc)


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../.. ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${UI_RESOURCES_DIR}/Build)
# TODO : "${UI_RESOURCES_DIR}/Build" is a HACK ! Fix this once we use .pc files
SET(link_libraries ${LINK_LIBRARIES} ${backend_LIBRARIES})

ADD_LIBRARY(ui_aura SHARED $<TARGET_OBJECTS:wm> $<TARGET_OBJECTS:native_theme> $<TARGET_OBJECTS:extra> $<TARGET_OBJECTS:client> $<TARGET_OBJECTS:misc> ${BACKEND_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_aura ${link_libraries})

INSTALL(TARGETS ui_aura
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
