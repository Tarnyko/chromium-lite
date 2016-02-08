PROJECT(chromium-ui-gl CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI GL")

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

# Mandatory dependencies
PKG_CHECK_MODULES(backend REQUIRED glesv2)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES gl_context_ozone.cc gl_implementation_ozone.cc gl_surface_ozone.cc gl_image_ozone_native_pixmap.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xcomposite xext)
  SET(BACKEND_SOURCE_FILES gl_context_x11.cc gl_implementation_x11.cc gl_surface_x11.cc gl_bindings_autogen_glx.cc gl_context_glx.cc gl_glx_api_implementation.cc gl_image_glx.cc gl_surface_egl_x11.cc gl_surface_glx.cc )
  ADD_DEFINITIONS(-DUSE_X11 -DGL_GLEXT_PROTOTYPES)
ENDIF(BACKEND STREQUAL "X11")

IF(BACKEND STREQUAL "OSMESA")
  SET(BACKEND_SOURCE_FILES gl_bindings_autogen_osmesa.cc gl_context_osmesa.cc gl_osmesa_api_implementation.cc gl_surface_osmesa.cc)
ENDIF(BACKEND STREQUAL "OSMESA")


# Common definitions
ADD_DEFINITIONS(-DGL_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT egl_util.cc gl_bindings.cc gl_bindings_autogen_egl.cc gl_bindings_autogen_gl.cc gl_bindings_skia_in_process.cc gl_context.cc gl_context_egl.cc gl_enums.cc gl_fence.cc gl_fence_arb.cc gl_fence_egl.cc gl_fence_nv.cc gl_egl_api_implementation.cc gl_gl_api_implementation.cc gl_helper.cc gl_image_memory.cc gl_image_ref_counted_memory.cc gl_image_shared_memory.cc gl_image_egl.cc gl_implementation.cc gl_share_group.cc gl_state_restorer.cc gl_surface.cc gl_surface_egl.cc gl_surface_overlay.cc gl_switches.cc gl_version_info.cc gpu_switching_manager.cc gpu_timing.cc scoped_api.cc scoped_binders.cc scoped_make_current.cc sync_control_vsync_provider.cc trace_util.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${glesv2_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${glesv2_LIBRARIES} ${backend_LIBRARIES} -ldl)

ADD_LIBRARY(ui_gl SHARED $<TARGET_OBJECTS:src> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_gl ${link_libraries})

INSTALL(TARGETS ui_gl
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
