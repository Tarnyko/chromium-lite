PROJECT(chromium-media-blink CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Media Blink")

# Location of third-party dependencies
SET(BLINK_DIR "${CMAKE_SOURCE_DIR}/../../third_party/WebKit")

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
ADD_DEFINITIONS(-DMEDIA_BLINK_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST
ADD_LIBRARY(src OBJECT active_loader.cc buffered_data_source.cc buffered_data_source_host_impl.cc buffered_resource_loader.cc cache_util.cc cdm_result_promise_helper.cc cdm_session_adapter.cc key_system_config_selector.cc multibuffer.cc multibuffer_data_source.cc multibuffer_reader.cc new_session_cdm_result_promise.cc resource_multibuffer_data_provider.cc texttrack_impl.cc url_index.cc video_frame_compositor.cc webaudiosourceprovider_impl.cc webcontentdecryptionmodule_impl.cc webcontentdecryptionmoduleaccess_impl.cc webcontentdecryptionmodulesession_impl.cc webencryptedmediaclient_impl.cc webinbandtexttrack_impl.cc webmediaplayer_params.cc webmediaplayer_util.cc webmediasource_impl.cc websourcebuffer_impl.cc)
# "encrypted_media_player_support.cc" and "webmediaplayer_impl.cc" are disabled
# because they pull FFmpeg


# all files refer to "media/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${BLINK_DIR} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/../..)

ADD_LIBRARY(media_blink SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(media_blink ${link_libraries})

INSTALL(TARGETS media_blink
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
