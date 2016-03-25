PROJECT(chromium-ozone C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Ozone-Wayland")

# Location of direct dependencies
SET(UI_DISPLAY_DIR "${CMAKE_SOURCE_DIR}/../ui/display")
SET(UI_OZONE_DIR "${CMAKE_SOURCE_DIR}/../ui/ozone")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(wayland REQUIRED wayland-client>=1.4.0 wayland-cursor>=1.4.0)
PKG_CHECK_MODULES(egl REQUIRED egl>=9.1.3 wayland-egl>=9.1.3)
PKG_CHECK_MODULES(xkbcommon REQUIRED xkbcommon)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Common definitions
ADD_DEFINITIONS(-DOZONE_BASE_IMPLEMENTATION -DOZONE_IMPLEMENTATION -DDISPLAY_TYPES_IMPLEMENTATION -DDISPLAY_UTIL_IMPLEMENTATION -DOZONE_WAYLAND_IMPLEMENTATION -DUSE_OZONE -DUSE_AURA)
# This is required by some versions of Mesa
ADD_DEFINITIONS(-DMESA_EGL_NO_X11_HEADERS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .CC,.H (UI_OZONE)

MESSAGE(STATUS "Generating .cc,.h files from the Wayland platform...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/ozone/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${UI_OZONE_DIR}/generate_ozone_platform_list.py --output_cc=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.cc --output_h=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.h --output_txt=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.txt --default=wayland wayland WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python ${UI_OZONE_DIR}/generate_constructor_list.py --platform_list=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.txt --output_cc=${CMAKE_BINARY_DIR}/ui/ozone/constructor_list.cc --namespace=ui --typename=OzonePlatform --typename=ClientNativePixmapFactory --include=\"ui/ozone/public/ozone_platform.h\" --include=\"ui/ozone/public/client_native_pixmap_factory.h\" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# platform
ADD_LIBRARY(platform OBJECT media/media_ozone_platform_wayland.cc platform/client_native_pixmap_factory_wayland.cc platform/message_generator.cc platform/ozone_gpu_platform_support_host.cc platform/ozone_platform_wayland.cc platform/ozone_wayland_window.cc platform/window_manager_wayland.cc)

# wayland
ADD_LIBRARY(wayland OBJECT wayland/data_device.cc wayland/data_offer.cc wayland/display.cc wayland/display_poll_thread.cc wayland/ozone_wayland_screen.cc wayland/screen.cc wayland/seat.cc wayland/window.cc wayland/egl/egl_window.cc wayland/egl/surface_ozone_wayland.cc wayland/input/cursor.cc wayland/input/keyboard.cc wayland/input/pointer.cc wayland/input/text_input.cc wayland/input/touchscreen.cc wayland/shell/shell.cc wayland/shell/shell_surface.cc wayland/shell/wl_shell_surface.cc wayland/shell/xdg_shell_surface.cc wayland/shell/ivi_shell_surface.cc)

# wayland_protocols
ADD_LIBRARY(wayland_protocols OBJECT wayland/protocol/text-protocol.c wayland/protocol/ivi-application-protocol.c wayland/protocol/xdg-shell-protocol.c)

# ui_ozone
ADD_LIBRARY(ui_ozone OBJECT ${UI_OZONE_DIR}/common/gpu/ozone_gpu_message_params.cc ${UI_OZONE_DIR}/common/stub_client_native_pixmap_factory.cc ${UI_OZONE_DIR}/common/stub_overlay_manager.cc ${UI_OZONE_DIR}/common/native_display_delegate_ozone.cc ${UI_OZONE_DIR}/common/display_mode_proxy.cc ${UI_OZONE_DIR}/common/display_snapshot_proxy.cc ${UI_OZONE_DIR}/common/display_util.cc ${UI_OZONE_DIR}/public/client_native_pixmap_factory.cc ${UI_OZONE_DIR}/public/gpu_platform_support.cc ${UI_OZONE_DIR}/public/gpu_platform_support_host.cc ${UI_OZONE_DIR}/public/surface_factory_ozone.cc ${UI_OZONE_DIR}/public/surface_ozone_egl.cc ${UI_OZONE_DIR}/public/overlay_candidates_ozone.cc ${UI_OZONE_DIR}/public/ozone_switches.cc ${UI_OZONE_DIR}/public/ozone_platform.cc ${UI_OZONE_DIR}/platform_selection.cc)

# ui_display
ADD_LIBRARY(ui_display OBJECT ${UI_DISPLAY_DIR}/types/display_mode.cc ${UI_DISPLAY_DIR}/types/display_snapshot.cc ${UI_DISPLAY_DIR}/util/display_util.cc ${UI_DISPLAY_DIR}/util/edid_parser.cc)

# generated (these are generated!)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/ozone/platform_list.cc ${CMAKE_BINARY_DIR}/ui/ozone/constructor_list.cc)


# all files refer to "ozone/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${wayland_INCLUDE_DIRS} ${egl_INCLUDE_DIRS} ${xkbcommon_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${LINK_LIBRARIES} ${wayland_LIBRARIES} ${egl_LIBRARIES} ${xkbcommon_LIBRARIES})

ADD_LIBRARY(ozone SHARED $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:wayland> $<TARGET_OBJECTS:wayland_protocols> $<TARGET_OBJECTS:ui_ozone> $<TARGET_OBJECTS:ui_display> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ozone ${link_libraries})

INSTALL(TARGETS ozone
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
