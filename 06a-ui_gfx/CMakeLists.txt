PROJECT(chromium-ui-gfx C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Gfx")

# Location of direct dependencies
SET(MOJO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../mojo")
SET(UI_MOJO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../mojo")

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
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(harfbuzz REQUIRED harfbuzz)

# libjpeg-turbo
IF(EXISTS "/usr/include/jpeglib.h")
  SET(HAVE_LIBJPEG_TURBO_H "1")
ENDIF()
CHECK_LIBRARY_EXISTS(jpeg jpeg_start_decompress "" HAVE_LIBJPEG_TURBO_SO)
IF(HAVE_LIBJPEG_TURBO_H)
  IF(HAVE_LIBJPEG_TURBO_SO)
    SET(HAVE_LIBJPEG_TURBO "1")
  ENDIF(HAVE_LIBJPEG_TURBO_SO)
ENDIF(HAVE_LIBJPEG_TURBO_H)
IF(NOT HAVE_LIBJPEG_TURBO)
  MESSAGE(FATAL_ERROR "\"jpeglib.h\" or \"libjpeg.so\" missing.
    Please install the \"libjpeg-turbo-devel\" or \"libjpeg62-turbo-dev\" package !")
ENDIF(NOT HAVE_LIBJPEG_TURBO)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  # useless, but this variable cannot be empty
  SET(BACKEND_SOURCE_FILES native_pixmap_handle_ozone.h)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xi)
  SET(BACKEND_SOURCE_FILES path_x11.cc x/x11_atom_cache.cc x/x11_connection.cc x/x11_error_tracker.cc x/x11_switches.cc x/x11_types.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DGFX_IMPLEMENTATION -DGFX_IPC_IMPLEMENTATION -DMOJO_GEOMETRY_IMPLEMENTATION -DUSE_AURA -DUSE_SYSTEM_LIBJPEG)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SOURCE_DIR}/public/tools/bindings/mojom_bindings_generator.py ${UI_MOJO_SOURCE_DIR}/geometry/geometry.mojom --use_bundled_pylibs -d .. -I. -o ${CMAKE_BINARY_DIR}/ui WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# PRE-GENERATION .ICON -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .icon files...")
 
EXECUTE_PROCESS(COMMAND mkdir -p ui/gfx/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# the final "/" at the end of working_directory is essential...
EXECUTE_PROCESS(COMMAND python vector_icons/aggregate_vector_icons.py --working_directory=vector_icons/ --output_cc=${CMAKE_BINARY_DIR}/ui/gfx/vector_icons.cc --output_h=${CMAKE_BINARY_DIR}/ui/gfx/vector_icons.h WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# geometry
ADD_LIBRARY(geometry OBJECT geometry/box_f.cc geometry/cubic_bezier.cc geometry/dip_util.cc geometry/insets.cc geometry/insets_f.cc geometry/matrix3_f.cc geometry/point.cc geometry/point3_f.cc geometry/point_conversions.cc geometry/point_f.cc geometry/quad_f.cc geometry/rect.cc geometry/rect_conversions.cc geometry/rect_f.cc geometry/scroll_offset.cc geometry/size.cc geometry/size_conversions.cc geometry/size_f.cc geometry/vector2d.cc geometry/vector2d_conversions.cc geometry/vector2d_f.cc geometry/vector3d_f.cc)

# animation
ADD_LIBRARY(animation OBJECT animation/animation.cc animation/animation_container.cc animation/linear_animation.cc animation/multi_animation.cc animation/slide_animation.cc animation/throb_animation.cc animation/tween.cc)

# codec
ADD_LIBRARY(codec OBJECT codec/jpeg_codec.cc codec/png_codec.cc)

# image
ADD_LIBRARY(image OBJECT image/canvas_image_source.cc image/image.cc image/image_family.cc image/image_png_rep.cc image/image_skia.cc image/image_skia_operations.cc image/image_skia_rep.cc image/image_util.cc)

# font
ADD_LIBRARY(font OBJECT font.cc font_fallback_linux.cc font_list.cc font_list_impl.cc font_render_params.cc font_render_params_linux.cc render_text.cc render_text_harfbuzz.cc harfbuzz_font_skia.cc hud_font.cc linux_font_delegate.cc platform_font_linux.cc)

# range
ADD_LIBRARY(range OBJECT range/range.cc range/range_f.cc)

# ipc
ADD_LIBRARY(ipc OBJECT ipc/gfx_param_traits.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})

# misc
ADD_LIBRARY(misc OBJECT blit.cc buffer_format_util.cc canvas.cc canvas_skia.cc color_analysis.cc color_profile.cc color_utils.cc display.cc display_change_notifier.cc display_observer.cc favicon_size.cc generic_shared_memory_id.cc gfx_paths.cc gpu_memory_buffer.cc interpolated_transform.cc nine_image_painter.cc paint_throbber.cc paint_vector_icon.cc path.cc screen.cc screen_aura.cc scrollbar_size.cc selection_model.cc sequential_id_generator.cc shadow_value.cc skbitmap_operations.cc skia_util.cc switches.cc text_elider.cc text_utils.cc text_utils_skia.cc transform.cc transform_util.cc ui_gfx_exports.cc utf16_indexing.cc)

# mojo_geometry
ADD_LIBRARY(mojo_geometry OBJECT ${MOJO_SOURCE_DIR}/converters/geometry/geometry_type_converters.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/mojo/geometry/geometry.mojom.cc ${CMAKE_BINARY_DIR}/ui/gfx/vector_icons.cc)


# all files refer to "ui/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${harfbuzz_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../.. ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${MOJO_SOURCE_DIR}/Build ${MOJO_SOURCE_DIR}/Build/mojo ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/ui)
# TODO : "${MOJO_SOURCE_DIR}/Build" is a HACK ! Fix this once we use .pc files
SET(link_libraries ${LINK_LIBRARIES} ${harfbuzz_LIBRARIES} ${backend_LIBRARIES} -ljpeg)

ADD_LIBRARY(ui_gfx SHARED $<TARGET_OBJECTS:geometry> $<TARGET_OBJECTS:animation> $<TARGET_OBJECTS:codec> $<TARGET_OBJECTS:image> $<TARGET_OBJECTS:font> $<TARGET_OBJECTS:range> $<TARGET_OBJECTS:ipc> $<TARGET_OBJECTS:backend> $<TARGET_OBJECTS:misc> $<TARGET_OBJECTS:mojo_geometry> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_gfx ${link_libraries})

INSTALL(TARGETS ui_gfx
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
