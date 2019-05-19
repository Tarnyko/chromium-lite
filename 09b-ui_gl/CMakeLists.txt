PROJECT(chromium-ui-gl CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI GL")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../../gpu")

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

# osmesa.h
IF(NOT EXISTS "/usr/${CMAKE_INSTALL_INCLUDEDIR}/GL/osmesa.h")
  MESSAGE(FATAL_ERROR "\"GL/osmesa.h\" missing.
    Please install the \"mesa-libOSMesa-devel\" or \"libosmesa*-dev\" packages !")
ENDIF()

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES init/gl_factory_ozone.cc init/gl_initializer_ozone.cc)
  ADD_DEFINITIONS(-DUSE_OZONE -DUSE_EGL)
# This is required by some versions of Mesa
  ADD_DEFINITIONS(-DMESA_EGL_NO_X11_HEADERS)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xcomposite xext)
  SET(BACKEND_SOURCE_FILES init/gl_factory_x11.cc init/gl_initializer_x11.cc gl_bindings_autogen_glx.cc gl_context_glx.cc gl_glx_api_implementation.cc gl_image_glx.cc gl_surface_egl_x11.cc gl_surface_glx.cc)
  ADD_DEFINITIONS(-DUSE_X11 -DUSE_GLX -DGL_GLEXT_PROTOTYPES)
ENDIF(BACKEND STREQUAL "X11")

# IF(BACKEND STREQUAL "OSMESA")
#  SET(BACKEND_SOURCE_FILES gl_bindings_autogen_osmesa.cc gl_implementation_osmesa.cc gl_osmesa_api_implementation.cc)
# ENDIF(BACKEND STREQUAL "OSMESA")


# Common definitions
ADD_DEFINITIONS(-DGL_IMPLEMENTATION -DGL_INIT_IMPLEMENTATION -DUSE_AURA -D_FILE_OFFSET_BITS=64)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT angle_platform_impl.cc egl_util.cc gl_bindings.cc gl_bindings_autogen_egl.cc gl_bindings_autogen_gl.cc gl_context.cc gl_context_egl.cc gl_context_stub.cc gl_enums.cc gl_fence.cc gl_fence_arb.cc gl_fence_egl.cc gl_fence_nv.cc gl_egl_api_implementation.cc gl_gl_api_implementation.cc gl_helper.cc gl_image.cc gl_image_memory.cc gl_image_ref_counted_memory.cc gl_image_shared_memory.cc gl_image_stub.cc gl_image_egl.cc gl_implementation.cc gl_share_group.cc gl_state_restorer.cc gl_surface.cc gl_surface_egl.cc gl_surface_format.cc gl_surface_overlay.cc gl_surface_stub.cc gl_stub_api.cc gl_stub_autogen_gl.cc gl_switches.cc gl_version_info.cc gpu_switching_manager.cc gpu_timing.cc scoped_binders.cc scoped_make_current.cc sync_control_vsync_provider.cc trace_util.cc gl_context_osmesa.cc gl_surface_osmesa.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "ui/...", so we include "../.." here
# we add local "CHROMIUM_GL_DIR" in front here, because its "eglext.h" header
# contains the EGL_SYNC_PRIOR_COMMANDS_IMPLICIT_EXTERNAL_ARM definition we need
SET(include_dirs ${INCLUDE_DIRS} ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${glesv2_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${glesv2_LIBRARIES} ${backend_LIBRARIES} -ldl)

ADD_LIBRARY(ui_gl SHARED $<TARGET_OBJECTS:src> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_gl ${LINK_LIBRARIES})

INSTALL(TARGETS ui_gl
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
