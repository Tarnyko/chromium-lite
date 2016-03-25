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
#
SET(GTEST_DIR "${CMAKE_SOURCE_DIR}/../../testing/gtest")
SET(UI_AURA_TEST_DIR "${CMAKE_SOURCE_DIR}/../../ui/aura/test")
SET(UI_VIEWS_TEST_DIR "${CMAKE_SOURCE_DIR}/../../ui/views/test")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../../gpu")

# Location of third-party dependencies
SET(DEVTOOLS_DISCOVERY_DIR "${CMAKE_SOURCE_DIR}/../../components/devtools_discovery")
SET(DEVTOOLS_HTTP_HANDLER_DIR "${CMAKE_SOURCE_DIR}/../../components/devtools_http_handler")
SET(WEB_CACHE_DIR "${CMAKE_SOURCE_DIR}/../../components/web_cache")

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
ADD_DEFINITIONS(-DCONTENT_SHELL_VERSION="99.77.34.5" -DTOOLKIT_VIEWS -DUSE_DEFAULT_RENDER_THEME -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .GRD -> .CC,.H,.PAK

MESSAGE(STATUS "Generating .cc,.h,.pak from .grd...")

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i shell_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR} --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# devtools_discovery
ADD_LIBRARY(devtools_discovery OBJECT ${DEVTOOLS_DISCOVERY_DIR}/basic_target_descriptor.cc ${DEVTOOLS_DISCOVERY_DIR}/devtools_discovery_manager.cc)

# devtools_http_handler
ADD_LIBRARY(devtools_http_handler OBJECT ${DEVTOOLS_HTTP_HANDLER_DIR}/devtools_http_handler.cc)

# web_cache
ADD_LIBRARY(web_cache OBJECT ${WEB_CACHE_DIR}/common/web_cache_message_generator.cc ${WEB_CACHE_DIR}/renderer/web_cache_render_process_observer.cc)

# ui_aura_test
ADD_LIBRARY(ui_aura_test OBJECT ${UI_AURA_TEST_DIR}/test_focus_client.cc ${UI_AURA_TEST_DIR}/test_screen.cc ${UI_AURA_TEST_DIR}/test_window_tree_client.cc)

# ui_views_test
ADD_LIBRARY(ui_views_test OBJECT ${UI_VIEWS_TEST_DIR}/desktop_test_views_delegate_aura.cc ${UI_VIEWS_TEST_DIR}/test_views_delegate_aura.cc)

# app
ADD_LIBRARY(app OBJECT app/shell_main.cc app/shell_main_delegate.cc)

# common
ADD_LIBRARY(common OBJECT common/shell_messages.cc common/shell_switches.cc common/shell_content_client.cc common/shell_test_configuration.cc)

# browser
ADD_LIBRARY(browser OBJECT browser/blink_test_controller.cc browser/shell_access_token_store.cc browser/shell_browser_context.cc browser/shell_content_browser_client.cc browser/shell_download_manager_delegate.cc browser/shell_devtools_frontend.cc browser/shell_devtools_manager_delegate.cc browser/shell_javascript_dialog_manager.cc browser/shell_quota_permission_context.cc browser/shell_permission_manager.cc browser/shell_speech_recognition_manager_delegate.cc browser/shell_url_request_context_getter.cc browser/shell_net_log.cc browser/shell_network_delegate.cc browser/notify_done_forwarder.cc browser/shell_resource_dispatcher_host_delegate.cc browser/shell_browser_main.cc browser/shell_browser_main_parts.cc browser/shell.cc browser/shell_platform_data_aura.cc browser/shell_views.cc)
# we could replace "shell_views.cc" with "shell_aura.cc" to not depend on Views

# renderer
ADD_LIBRARY(renderer OBJECT renderer/shell_content_renderer_client.cc renderer/shell_render_view_observer.cc)

# utility
ADD_LIBRARY(utility OBJECT utility/shell_content_utility_client.cc)


# all files refer to "content/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${GTEST_DIR}/include ${CHROMIUM_GL_DIR}/noninclude ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_EXT_DIR}/config ${BLINK_DIR} ${V8_DIR}/include ${UI_ACCESSIBILITY_DIR}/Build ${MOJO_DIR}/Build ${MOJO_DIR}/Build/mojo ${NET_DIR}/Build ${BLINK_DIR}/Source/Build ${CONTENT_DIR}/Build ${CONTENT_DIR}/Build/content ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../..)
# TODO : "${UI_ACCESSIBILITY_DIR}/${NET_DIR}/${BLINK_DIR}/${CONTENT_DIR}/Build" are HACKS !

ADD_EXECUTABLE(content_shell $<TARGET_OBJECTS:devtools_discovery> $<TARGET_OBJECTS:devtools_http_handler> $<TARGET_OBJECTS:web_cache> $<TARGET_OBJECTS:ui_aura_test> $<TARGET_OBJECTS:ui_views_test> $<TARGET_OBJECTS:app> $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:browser> $<TARGET_OBJECTS:renderer> $<TARGET_OBJECTS:utility>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(content_shell ${LINK_LIBRARIES}

INSTALL(TARGETS content_shell
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/chromium)
