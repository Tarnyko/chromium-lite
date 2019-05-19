PROJECT(chromium-ui-ozone C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Ozone")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../../third_party/khronos")

# Location of direct dependencies
SET(UI_BASE_DIR "${CMAKE_SOURCE_DIR}/../base")
SET(UI_DISPLAY_DIR "${CMAKE_SOURCE_DIR}/../display")
SET(UI_GFX_DIR "${CMAKE_SOURCE_DIR}/../gfx")
SET(MOJO_SRC_DIR "${CMAKE_SOURCE_DIR}/../../mojo")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

# Verify a backend (X11/WAYLAND) has been chosen ; default to WAYLAND
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/WAYLAND>\" not passed; defaulting to WAYLAND...")
  SET(BACKEND "WAYLAND")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies

IF(BACKEND STREQUAL "WAYLAND")
  PKG_CHECK_MODULES(backend REQUIRED wayland-client>=1.4.0 wayland-cursor>=1.4.0 wayland-protocols>=0.1.0 wayland-egl>=9.1.3 egl>=9.1.3 xkbcommon)
  EXECUTE_PROCESS(COMMAND sh -c "wayland-scanner client-header \"`pkg-config --variable=pkgdatadir wayland-protocols`\"/unstable/xdg-shell/xdg-shell-unstable-v5.xml xdg-shell-unstable-v5-client-protocol.h" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  EXECUTE_PROCESS(COMMAND sh -c "wayland-scanner code \"`pkg-config --variable=pkgdatadir wayland-protocols`\"/unstable/xdg-shell/xdg-shell-unstable-v5.xml xdg-shell-unstable-v5-protocol.c" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  SET(BACKEND_SOURCE_FILES platform/wayland/client_native_pixmap_factory_wayland.cc platform/wayland/gl_surface_wayland.cc platform/wayland/ozone_platform_wayland.cc platform/wayland/wayland_connection.cc platform/wayland/wayland_keyboard.cc platform/wayland/wayland_object.cc platform/wayland/wayland_output.cc platform/wayland/wayland_pointer.cc platform/wayland/wayland_surface_factory.cc platform/wayland/wayland_window.cc platform/wayland/wayland_xkb_keyboard_layout_engine.cc)
  ADD_DEFINITIONS(-DUSE_EGL)
  # This is required by some old versions of Mesa
  ADD_DEFINITIONS(-DMESA_EGL_NO_X11_HEADERS)
ENDIF(BACKEND STREQUAL "WAYLAND")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11)
  SET(BACKEND_SOURCE_FILES platform/x11/client_native_pixmap_factory_x11.cc platform/x11/gl_ozone_glx.cc platform/x11/gl_surface_glx_ozone.cc platform/x11/ozone_platform_x11.cc platform/x11/x11_cursor_factory_ozone.cc platform/x11/x11_surface_factory.cc)
  ADD_DEFINITIONS(-DUSE_GLX)
ENDIF(BACKEND STREQUAL "X11")

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Common definitions
ADD_DEFINITIONS(-DOZONE_BASE_IMPLEMENTATION -DOZONE_IMPLEMENTATION -DDISPLAY_TYPES_IMPLEMENTATION -DDISPLAY_UTIL_IMPLEMENTATION -DUSE_OZONE -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .CC,.H (PLATFORM_LIST)

MESSAGE(STATUS "Generating .cc,.h files from the selected platform...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/ozone/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python generate_ozone_platform_list.py --output_cc=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.cc --output_h=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.h --output_txt=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.txt --default=wayland wayland WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python generate_constructor_list.py --platform_list=${CMAKE_BINARY_DIR}/ui/ozone/platform_list.txt --output_cc=${CMAKE_BINARY_DIR}/ui/ozone/constructor_list.cc --namespace=ui --typename=OzonePlatform --typename=ClientNativePixmapFactory --include=\"ui/ozone/public/ozone_platform.h\" --include=\"ui/ozone/public/client_native_pixmap_factory.h\" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .TMPL -> .ZIP (bytecode)

MESSAGE(STATUS "Generating .zip files from .tmpl...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs precompile -o ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/ozone/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/device_cursor.mojom -d . -I../.. -o ${CMAKE_BINARY_DIR}/ui/ozone --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate public/interfaces/device_cursor.mojom -d . -I../.. -o ${CMAKE_BINARY_DIR}/ui/ozone --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++ WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# public
ADD_LIBRARY(public OBJECT public/client_native_pixmap_factory.cc public/cursor_factory_ozone.cc public/gpu_platform_support_host.cc public/input_controller.cc public/overlay_candidates_ozone.cc public/ozone_platform.cc public/ozone_switches.cc public/surface_factory_ozone.cc platform_selection.cc)

# common
ADD_LIBRARY(common OBJECT common/display_mode_proxy.cc common/display_snapshot_proxy.cc common/egl_util.cc common/gl_ozone_egl.cc common/stub_client_native_pixmap_factory.cc common/stub_overlay_manager.cc common/gpu/ozone_gpu_message_generator.cc common/gpu/ozone_gpu_message_params.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})

# generated (these are generated!)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/ozone/platform_list.cc ${CMAKE_BINARY_DIR}/ui/ozone/constructor_list.cc ${CMAKE_BINARY_DIR}/ui/ozone/public/interfaces/device_cursor.mojom.cc ${CMAKE_BINARY_DIR}/ui/ozone/public/interfaces/device_cursor.mojom-shared.cc ${CMAKE_BINARY_DIR}/xdg-shell-unstable-v5-protocol.c)


# we include CHROMIUM_GL_DIR before eventual EGL headers, because its "eglplatform.h" contain Ozone-specific definitions which only landed in Mesa 18.2.0 (2018-11)
# all files refer to "ui/ozone/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${CHROMIUM_GL_DIR} ${backend_INCLUDE_DIRS} ${UI_BASE_DIR} ${UI_GFX_DIR} ${MOJO_SRC_DIR} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/ui/ozone ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${LINK_LIBRARIES} ${backend_LIBRARIES})


ADD_LIBRARY(ui_ozone SHARED $<TARGET_OBJECTS:public> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:backend> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_ozone ${link_libraries})

INSTALL(TARGETS ui_ozone
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
