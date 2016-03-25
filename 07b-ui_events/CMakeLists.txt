PROJECT(chromium-ui-events CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Events")

# Location of direct dependencies
SET(UDEV_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../device/udev_linux")
SET(UDEV_EXT_DIR "${CMAKE_SOURCE_DIR}/../../third_party/libudev")
SET(TOOLS_DIR "${CMAKE_SOURCE_DIR}/../../tools")

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
  FIND_PROGRAM(PYTHON_FOUND python)
  IF(NOT PYTHON_FOUND)
    MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
  ELSE()
    MESSAGE(STATUS "\"python\" found...")
  ENDIF(NOT PYTHON_FOUND)
  PKG_CHECK_MODULES(backend REQUIRED libudev)

  MESSAGE(STATUS "Generating .cc,.h files for udev...")
  EXECUTE_PROCESS(COMMAND mkdir -p library_loaders/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  EXECUTE_PROCESS(COMMAND python ${TOOLS_DIR}/generate_library_loader/generate_library_loader.py --name LibUdev0Loader --output-h ${CMAKE_BINARY_DIR}/library_loaders/libudev0.h --output-cc ${CMAKE_BINARY_DIR}/libudev0_loader.cc --header \"${UDEV_EXT_DIR}/libudev0.h\" --link-directly=0 udev_device_get_action udev_device_get_devnode udev_device_get_parent udev_device_get_parent_with_subsystem_devtype udev_device_get_property_value udev_device_get_subsystem udev_device_get_sysattr_value udev_device_get_sysname udev_device_get_syspath udev_device_new_from_devnum udev_device_new_from_subsystem_sysname udev_device_new_from_syspath udev_device_unref udev_enumerate_add_match_subsystem udev_enumerate_get_list_entry udev_enumerate_new udev_enumerate_scan_devices udev_enumerate_unref udev_list_entry_get_next udev_list_entry_get_name udev_monitor_enable_receiving udev_monitor_filter_add_match_subsystem_devtype udev_monitor_get_fd udev_monitor_new_from_netlink udev_monitor_receive_device udev_monitor_unref udev_new udev_set_log_fn udev_set_log_priority udev_unref WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  EXECUTE_PROCESS(COMMAND python ${TOOLS_DIR}/generate_library_loader/generate_library_loader.py --name LibUdev1Loader --output-h ${CMAKE_BINARY_DIR}/library_loaders/libudev1.h --output-cc ${CMAKE_BINARY_DIR}/libudev1_loader.cc --header \"${UDEV_EXT_DIR}/libudev1.h\" --link-directly=0 udev_device_get_action udev_device_get_devnode udev_device_get_parent udev_device_get_parent_with_subsystem_devtype udev_device_get_property_value udev_device_get_subsystem udev_device_get_sysattr_value udev_device_get_sysname udev_device_get_syspath udev_device_new_from_devnum udev_device_new_from_subsystem_sysname udev_device_new_from_syspath udev_device_unref udev_enumerate_add_match_subsystem udev_enumerate_get_list_entry udev_enumerate_new udev_enumerate_scan_devices udev_enumerate_unref udev_list_entry_get_next udev_list_entry_get_name udev_monitor_enable_receiving udev_monitor_filter_add_match_subsystem_devtype udev_monitor_get_fd udev_monitor_new_from_netlink udev_monitor_receive_device udev_monitor_unref udev_new udev_set_log_fn udev_set_log_priority udev_unref WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

  SET(BACKEND_SOURCE_FILES ozone/events_ozone.cc events_default.cc ozone/evdev/device_event_dispatcher_evdev.cc ozone/evdev/event_converter_evdev.cc ozone/evdev/event_converter_evdev_impl.cc ozone/evdev/event_device_info.cc ozone/evdev/event_factory_evdev.cc ozone/evdev/event_modifiers_evdev.cc ozone/evdev/event_thread_evdev.cc ozone/evdev/input_controller_evdev.cc ozone/evdev/input_device_factory_evdev.cc ozone/evdev/input_device_factory_evdev_proxy.cc ozone/evdev/input_device_settings_evdev.cc ozone/evdev/input_injector_evdev.cc ozone/evdev/keyboard_evdev.cc ozone/evdev/keyboard_util_evdev.cc ozone/evdev/mouse_button_map_evdev.cc ozone/evdev/tablet_event_converter_evdev.cc ozone/evdev/touch_evdev_types.cc ozone/evdev/touch_evdev_debug_buffer.cc ozone/evdev/touch_event_converter_evdev.cc ozone/evdev/touch_noise/far_apart_taps_touch_noise_filter.cc ozone/evdev/touch_noise/horizontally_aligned_touch_noise_filter.cc ozone/evdev/touch_noise/single_position_touch_noise_filter.cc ozone/evdev/touch_noise/touch_noise_finder.cc ozone/layout/keyboard_layout_engine_manager.cc ozone/layout/layout_util.cc ozone/layout/no/no_keyboard_layout_engine.cc ozone/layout/stub/stub_keyboard_layout_engine.cc ozone/layout/xkb/xkb_evdev_codes.cc ozone/layout/xkb/xkb_keyboard_layout_engine.cc ozone/device/device_event.cc ozone/device/device_manager.cc ozone/device/device_manager_manual.cc ozone/device/udev/device_manager_udev.cc platform/platform_event_source_stub.cc ${UDEV_SOURCE_DIR}/udev.cc ${UDEV_SOURCE_DIR}/udev0_loader.cc ${UDEV_SOURCE_DIR}/udev1_loader.cc ${UDEV_SOURCE_DIR}/udev_loader.cc ${CMAKE_BINARY_DIR}/libudev0_loader.cc ${CMAKE_BINARY_DIR}/libudev1_loader.cc)
  ADD_DEFINITIONS(-DUSE_OZONE -DEVENTS_OZONE_IMPLEMENTATION -DEVENTS_OZONE_EVDEV_IMPLEMENTATION -DEVENTS_OZONE_LAYOUT_IMPLEMENTATION -DUSE_XKBCOMMON -DUSE_UDEV -DUSE_EVDEV)
  # Let us avoid errors with "PRIx*" macros within Ozone-Evdev code
  ADD_DEFINITIONS(-D__STDC_FORMAT_MACROS)
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
SET(include_dirs ${INCLUDE_DIRS} ${xkbcommon_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${LINK_LIBRARIES} ${xkbcommon_LIBRARIES} ${backend_LIBRARIES})

ADD_LIBRARY(ui_events SHARED $<TARGET_OBJECTS:keycode_converter> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:ipc> $<TARGET_OBJECTS:devices> $<TARGET_OBJECTS:gesture_detection> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_events ${link_libraries})

INSTALL(TARGETS ui_events
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
