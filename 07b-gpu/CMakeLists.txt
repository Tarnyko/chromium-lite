PROJECT(chromium-gpu CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - GPU")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../skia")

# Location of required Angle dependency
SET(ANGLE_SOURCE_DIR "${CMAKE_SOURCE_DIR}../third_party/angle")

# Variable paths for required third-parties
SET(PROTOC_WRAPPER_DIR "..")

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
FIND_PROGRAM(PROTOC_FOUND protoc)
IF(NOT PROTOC_FOUND)
  MESSAGE(FATAL_ERROR "\"protoc\" is required, please install the \"protobuf-compiler\" package !")
ELSE()
  MESSAGE(STATUS "\"protoc\" found...")
ENDIF(NOT PROTOC_FOUND)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES config/gpu_info_collector_ozone.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xext)
  SET(BACKEND_SOURCE_FILES config/gpu_info_collector_x11.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DGLES2_IMPL_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .PROTO -> .CC,.H,.PY

MESSAGE(STATUS "Generating .cc,.h files from .proto...")

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --proto-in-dir command_buffer/service --proto-in-file disk_cache_proto.proto --use-system-protobuf=1 -- protoc --cpp_out command_buffer/service WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# command_buffer_client
ADD_LIBRARY(command_buffer_client OBJECT command_buffer/client/cmd_buffer_helper.cc command_buffer/client/fenced_allocator.cc command_buffer/client/gpu_memory_buffer_manager.cc command_buffer/client/mapped_memory.cc command_buffer/client/ring_buffer.cc command_buffer/client/transfer_buffer.cc command_buffer/client/gles2_cmd_helper.cc)

# command_buffer_service
ADD_LIBRARY(command_buffer_service OBJECT command_buffer/service/buffer_manager.cc command_buffer/service/cmd_parser.cc command_buffer/service/command_buffer_service.cc command_buffer/service/common_decoder.cc command_buffer/service/context_group.cc command_buffer/service/context_state.cc command_buffer/service/error_state.cc command_buffer/service/feature_info.cc command_buffer/service/framebuffer_completeness_cache.cc command_buffer/service/framebuffer_manager.cc command_buffer/service/gl_context_virtual.cc command_buffer/service/gl_state_restorer_impl.cc command_buffer/service/gles2_cmd_clear_framebuffer.cc command_buffer/service/gles2_cmd_copy_texture_chromium command_buffer/service/gles2_cmd_decoder.cc command_buffer/service/gles2_cmd_validation.cc command_buffer/service/gpu_scheduler.cc command_buffer/service/gpu_scheduler.cc command_buffer/service/gpu_state_tracer.cc command_buffer/service/gpu_switches.cc command_buffer/service/gpu_tracer.cc command_buffer/service/id_manager.cc command_buffer/service/image_factory.cc command_buffer/service/image_manager.cc command_buffer/service/in_process_command_buffer.cc command_buffer/service/logger.cc command_buffer/service/mailbox_manager.cc command_buffer/service/mailbox_manager_impl.cc command_buffer/service/mailbox_manager_sync.cc command_buffer/service/memory_program_cache.cc command_buffer/service/program_cache.cc command_buffer/service/path_manager.cc command_buffer/service/program_manager.cc command_buffer/service/query_manager.cc command_buffer/service/query_manager.cc command_buffer/service/renderbuffer_manager.cc command_buffer/service/sampler_manager.cc command_buffer/service/shader_manager.cc command_buffer/service/shader_translator.cc command_buffer/service/shader_translator_cache.cc command_buffer/service/sync_point_manager.cc command_buffer/service/texture_definition.cc command_buffer/service/texture_manager.cc command_buffer/service/transfer_buffer_manager.cc command_buffer/service/valuebuffer_manager.cc command_buffer/service/vertex_array_manager.cc command_buffer/service/vertex_attrib_manager.cc)

# config
ADD_LIBRARY(config OBJECT config/dx_diag_node.cc config/gpu_blacklist.cc config/gpu_control_list.cc config/gpu_driver_bug_list.cc config/gpu_driver_bug_list_json.cc config/gpu_info.cc config/gpu_info_collector.cc config/gpu_info_collector_linux.cc config/gpu_switches.cc config/gpu_test_config.cc config/gpu_test_expectations_parser.cc config/gpu_util.cc config/software_rendering_list_json.cc)

# ipc
ADD_LIBRARY(ipc OBJECT ipc/gpu_command_buffer_traits.cc)

# generated (this is generated !)
ADD_LIBRARY(generated OBJECT command_buffer/service/disk_cache_proto.pb.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "gpu/...", so we include "../" here
# we add "noninclude" in front here, because its "glext.h" header
# contains the GL_CONTEXT_LOST_KHR definition we need
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CHROMIUM_GL_DIR}/noninclude ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${backend_LIBRARIES})

ADD_LIBRARY(gpu SHARED $<TARGET_OBJECTS:command_buffer_client> $<TARGET_OBJECTS:command_buffer_service> $<TARGET_OBJECTS:config> $<TARGET_OBJECTS:ipc> $<TARGET_OBJECTS:generated> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(gpu ${link_libraries})

INSTALL(TARGETS gpu
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
