PROJECT(chromium-ozone C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Ozone")

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

# Common definitions
ADD_DEFINITIONS(-DOZONE_WAYLAND_IMPLEMENTATION -DUSE_OZONE -DUSE_AURA)
# This is required by some versions of Mesa
ADD_DEFINITIONS(-DMESA_EGL_NO_X11_HEADERS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(platform OBJECT media/media_ozone_platform_wayland.cc platform/client_native_pixmap_factory_wayland.cc platform/message_generator.cc platform/ozone_platform_wayland.cc platform/ozone_wayland_window.cc platform/window_manager_wayland.cc)

ADD_LIBRARY(wayland OBJECT wayland/data_device.cc wayland/data_offer.cc wayland/display.cc wayland/display_poll_thread.cc wayland/ozone_wayland_screen.cc wayland/screen.cc wayland/seat.cc wayland/window.cc wayland/egl/egl_window.cc wayland/egl/surface_ozone_wayland.cc wayland/input/cursor.cc wayland/input/keyboard.cc wayland/input/pointer.cc wayland/input/text_input.cc wayland/input/touchscreen.cc wayland/shell/shell.cc wayland/shell/shell_surface.cc wayland/shell/wl_shell_surface.cc wayland/shell/xdg_shell_surface.cc wayland/shell/ivi_shell_surface.cc)

ADD_LIBRARY(wayland_protocols OBJECT wayland/protocol/text-protocol.c wayland/protocol/ivi-application-protocol.c wayland/protocol/xdg-shell-protocol.c)


# all files refer to "ozone/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${wayland_INCLUDE_DIRS} ${egl_INCLUDE_DIRS} ${xkbcommon_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${wayland_LIBRARIES} ${egl_LIBRARIES} ${xkbcommon_LIBRARIES})

ADD_LIBRARY(ozone SHARED $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:wayland> $<TARGET_OBJECTS:wayland_protocols>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ozone ${link_libraries})

INSTALL(TARGETS ozone
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
