PROJECT(chromium-gpu-command-buffer CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - GPU Command Buffer")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/..")

# CMake modules
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DGLES2_UTILS_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(common OBJECT common/buffer.cc common/capabilities.cc common/cmd_buffer_common.cc common/debug_marker_manager.cc common/gles2_cmd_format.cc common/gles2_cmd_utils.cc common/id_allocator.cc common/mailbox.cc common/mailbox_holder.cc common/value_state.cc)


# all files refer to "gpu/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${CMAKE_SOURCE_DIR}/../..)

ADD_LIBRARY(gpu_command_buffer SHARED $<TARGET_OBJECTS:common>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(gpu_command_buffer ${link_libraries})

INSTALL(TARGETS gpu_command_buffer
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
