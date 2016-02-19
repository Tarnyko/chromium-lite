PROJECT(chromium-ui-events CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Events")

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
PKG_CHECK_MODULES(xkbcommon REQUIRED xkbcommon)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  PKG_CHECK_MODULES(backend REQUIRED libudev)
  SET(BACKEND_SOURCE_FILES ozone/events_ozone.cc events_default.cc ozone/device/device_event.cc ozone/device/device_manager.cc ozone/device/device_manager_manual.cc ozone/device/udev/device_manager_udev.cc platform/platform_event_source_stub.cc)
  ADD_DEFINITIONS(-DUSE_OZONE -DEVENTS_OZONE_IMPLEMENTATION -DUSE_UDEV)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xi xproto inputproto libevent)
  SET(BACKEND_SOURCE_FILES keycodes/keyboard_code_conversion_x.cc x/keysym_to_unicode.cc x/events_x.cc devices/x11/device_data_manager_x11.cc devices/x11/device_list_cache_x11.cc devices/x11/touch_factory_x11.cc platform/x11/x11_event_source.cc platform/x11/x11_event_source_libevent.cc platform/x11/x11_hotplug_event_handler.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DEVENTS_IMPLEMENTATION -DEVENTS_BASE_IMPLEMENTATION -DEVENTS_IPC_IMPLEMENTATION -DEVENTS_DEVICES_IMPLEMENTATION -DGESTURE_DETECTION_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# keycode_converter
ADD_LIBRARY(keycode_converter OBJECT keycodes/dom/keycode_converter.cc)

# base
ADD_LIBRARY(base OBJECT base_event_utils.cc event.cc event_dispatcher.cc event_handler.cc event_processor.cc event_source.cc event_switches.cc event_target.cc event_utils.cc gesture_event_details.cc gestures/fling_curve.cc gestures/gesture_provider_aura.cc gestures/gesture_recognizer_impl.cc gestures/motion_event_aura.cc keycodes/keyboard_code_conversion.cc keycodes/keyboard_code_conversion_xkb.cc linux/text_edit_command_auralinux.cc linux/text_edit_key_bindings_delegate_auralinux.cc latency_info.cc null_event_targeter.cc scoped_target_handler.cc)

# ipc
ADD_LIBRARY(ipc OBJECT ipc/latency_info_param_traits.cc)

# devices
ADD_LIBRARY(devices OBJECT devices/device_data_manager.cc devices/device_util_linux.cc devices/input_device.cc devices/keyboard_device.cc devices/touchscreen_device.cc)

# gesture_detection
ADD_LIBRARY(gesture_detection OBJECT gesture_detection/filtered_gesture_provider.cc gesture_detection/gesture_configuration.cc gesture_detection/gesture_configuration_aura.cc gesture_detection/gesture_detector.cc gesture_detection/gesture_event_data.cc gesture_detection/gesture_event_data_packet.cc gesture_detection/gesture_listeners.cc gesture_detection/gesture_provider.cc gesture_detection/gesture_provider_config_helper.cc gesture_detection/gesture_touch_uma_histogram.cc gesture_detection/motion_event.cc gesture_detection/motion_event_buffer.cc gesture_detection/motion_event_generic.cc gesture_detection/scale_gesture_detector.cc gesture_detection/scale_gesture_listeners.cc gesture_detection/snap_scroll_controller.cc gesture_detection/touch_disposition_gesture_filter.cc gesture_detection/velocity_tracker.cc gesture_detection/velocity_tracker_state.cc)

# platform
ADD_LIBRARY(platform OBJECT platform/platform_event_source.cc platform/scoped_event_dispatcher.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${xkbcommon_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${xkbcommon_LIBRARIES} ${backend_LIBRARIES})

ADD_LIBRARY(ui_events SHARED $<TARGET_OBJECTS:keycode_converter> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:ipc> $<TARGET_OBJECTS:devices> $<TARGET_OBJECTS:gesture_detection> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_events ${link_libraries})

INSTALL(TARGETS ui_events
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
