PROJECT(chromium-cc-blink CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - CC Blink")

# Location of third-party dependencies
SET(BLINK_DIR "${CMAKE_SOURCE_DIR}/../../third_party/WebKit")

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
INCLUDE(GNUInstallDirs)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DCC_BLINK_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT scrollbar_impl.cc web_animation_curve_common.cc web_animation_impl.cc web_compositor_animation_player_impl.cc web_compositor_animation_timeline_impl.cc web_compositor_support_impl.cc web_content_layer_impl.cc web_display_item_list_impl.cc web_external_bitmap_impl.cc web_external_texture_layer_impl.cc web_filter_animation_curve_impl.cc web_filter_operations_impl.cc web_float_animation_curve_impl.cc web_image_layer_impl.cc web_layer_impl.cc web_layer_impl_fixed_bounds.cc web_scroll_offset_animation_curve_impl.cc web_scrollbar_layer_impl.cc web_to_cc_animation_delegate_adapter.cc web_transform_animation_curve_impl.cc web_transform_operations_impl.cc)


# all files refer to "cc/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${BLINK_DIR} ${CHROMIUM_GL_DIR}/noninclude ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/../..)

ADD_LIBRARY(cc_blink SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(cc_blink ${link_libraries})

INSTALL(TARGETS cc_blink
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
