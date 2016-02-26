PROJECT(chromium-content-shell CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Content Shell")

# Location of direct dependencies
SET(V8_DIR "${CMAKE_SOURCE_DIR}/../../v8")
SET(NET_DIR "${CMAKE_SOURCE_DIR}/../../net")
SET(MOJO_DIR "${CMAKE_SOURCE_DIR}/../../mojo")
SET(UI_ACCESSIBILITY_DIR "${CMAKE_SOURCE_DIR}/../../ui/accessibility")
SET(BLINK_DIR "${CMAKE_SOURCE_DIR}/../../third_party/WebKit")
SET(CONTENT_DIR "${CMAKE_SOURCE_DIR}/..")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/../..")

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
ADD_DEFINITIONS(-DCONTENT_SHELL_VERSION="99.77.34.5" -DTOOLKIT_VIEWS -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .GRD -> .CC,.H,.PAK

MESSAGE(STATUS "Generating .cc,.h,.pak from .grd...")

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i shell_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR} --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# app
ADD_LIBRARY(app OBJECT app/shell_main.cc app/shell_main_delegate.cc)

# common
ADD_LIBRARY(common OBJECT common/shell_content_client.cc common/shell_messages.cc common/shell_switches.cc common/shell_test_configuration.cc common/layout_test/layout_test_messages.cc)

# browser
ADD_LIBRARY(browser OBJECT browser/blink_test_controller.cc browser/notify_done_forwarder.cc browser/shell.cc browser/shell_access_token_store.cc browser/shell_browser_context.cc browser/shell_browser_main.cc browser/shell_browser_main_parts.cc browser/shell_content_browser_client.cc browser/shell_devtools_frontend.cc browser/shell_devtools_manager_delegate.cc browser/shell_download_manager_delegate.cc browser/shell_javascript_dialog_manager.cc browser/shell_login_dialog.cc browser/shell_net_log.cc browser/shell_network_delegate.cc browser/shell_permission_manager.cc browser/shell_platform_data_aura.cc browser/shell_plugin_service_filter.cc browser/shell_quota_permission_context.cc browser/shell_resource_dispatcher_host_delegate.cc browser/shell_speech_recognition_manager_delegate.cc browser/shell_url_request_context_getter.cc browser/layout_test/layout_test_browser_context.cc browser/layout_test/layout_test_browser_main.cc browser/layout_test/layout_test_browser_main_parts.cc browser/layout_test/layout_test_content_browser_client.cc browser/layout_test/layout_test_devtools_frontend.cc browser/layout_test/layout_test_download_manager_delegate.cc browser/layout_test/layout_test_javascript_dialog_manager.cc browser/layout_test/layout_test_message_filter.cc browser/layout_test/layout_test_navigator_connect_service_factory.cc browser/layout_test/layout_test_notification_manager.cc browser/layout_test/layout_test_permission_manager.cc browser/layout_test/layout_test_push_messaging_service.cc browser/layout_test/layout_test_resource_dispatcher_host_delegate.cc browser/layout_test/layout_test_url_request_context_getter.cc browser/shell_views.cc)
# we could replace "shell_views.cc" with "shell_aura.cc" to not depend on Views

# renderer
ADD_LIBRARY(renderer OBJECT renderer/shell_content_renderer_client.cc renderer/shell_render_view_observer.cc renderer/layout_test/blink_test_helpers.cc renderer/layout_test/blink_test_runner.cc renderer/layout_test/layout_test_content_renderer_client.cc renderer/layout_test/layout_test_render_frame_observer.cc renderer/layout_test/layout_test_render_process_observer.cc renderer/layout_test/leak_detector.cc renderer/layout_test/test_media_stream_renderer_factory.cc renderer/layout_test/test_video_frame_provider.cc)

# utility
ADD_LIBRARY(utility OBJECT utility/shell_content_utility_client.cc)


# all files refer to "content/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${BLINK_DIR} ${V8_DIR}/include ${UI_ACCESSIBILITY_DIR}/Build ${MOJO_DIR}/Build ${MOJO_DIR}/Build/mojo ${NET_DIR}/Build ${BLINK_DIR}/Source/Build ${CONTENT_DIR}/Build ${CONTENT_DIR}/Build/content ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../..)
# TODO : "${UI_ACCESSIBILITY_DIR}/${NET_DIR}/${BLINK_DIR}/${CONTENT_DIR}/Build" are HACKS !

ADD_EXECUTABLE(content_shell $<TARGET_OBJECTS:app> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:browser> $<TARGET_OBJECTS:renderer> $<TARGET_OBJECTS:utility>)
INCLUDE_DIRECTORIES(${include_dirs})

INSTALL(TARGETS content_shell
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
