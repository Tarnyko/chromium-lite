PROJECT(chromium-ui-base CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Base")

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

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES clipboard/clipboard_aura.cc cursor/cursor_loader_ozone.cc cursor/cursor_ozone.cc cursor/ozone/bitmap_cursor_factory_ozone.cc dragdrop/os_exchange_data_provider_aura.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xcursor xext xfixes xrender xscrnsaver)
  SET(BACKEND_SOURCE_FILES clipboard/clipboard_aurax11.cc cursor/cursor_loader_x11.cc cursor/cursor_x11.cc dragdrop/os_exchange_data_provider_aurax11.cc idle/idle_query_x11.cc idle/screensaver_window_finder_x11.cc x/selection_owner.cc x/selection_requestor.cc x/selection_utils.cc x/x11_foreign_window_manager.cc x/x11_menu_list.cc x/x11_util.cc)
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DUI_BASE_IMPLEMENTATION -DUI_BASE_IME_IMPLEMENTATION -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# base
ADD_LIBRARY(base OBJECT base_window.cc default_theme_provider.cc device_form_factor_desktop.cc idle/idle.cc idle/idle_linux.cc layout.cc nine_image_painter_factory.cc page_transition_types.cc template_expressions.cc text/bytes_formatting.cc theme_provider.cc user_activity/user_activity_detector.cc view_prop.cc webui/jstemplate_builder.cc webui/web_ui_util.cc window_open_disposition.cc ui_base_exports.cc ui_base_paths.cc ui_base_switches.cc ui_base_switches_util.cc ui_base_types.cc)

# accelerators
ADD_LIBRARY(accelerators OBJECT accelerators/accelerator.cc accelerators/accelerator_history.cc accelerators/accelerator_manager.cc accelerators/menu_label_accelerator_util_linux.cc)

# clipboard
ADD_LIBRARY(clipboard OBJECT clipboard/clipboard.cc clipboard/clipboard_constants.cc clipboard/custom_data_helper.cc clipboard/custom_data_helper_linux.cc clipboard/scoped_clipboard_writer.cc)

# cursor
ADD_LIBRARY(cursor OBJECT cursor/cursor.cc cursor/cursor_util.cc cursor/cursors_aura.cc cursor/image_cursors.cc)

# dragdrop
ADD_LIBRARY(dragdrop OBJECT dragdrop/drag_utils.cc dragdrop/drag_utils_aura.cc dragdrop/drop_target_event.cc dragdrop/file_info.cc dragdrop/os_exchange_data.cc)

# l10n
ADD_LIBRARY(l10n OBJECT l10n/l10n_font_util.cc l10n/l10n_util.cc l10n/l10n_util_posix.cc)
# we let out "l10n/formatter.cc" and "l10n/time_format.cc" because
# they need the libicu fork

# models
ADD_LIBRARY(models OBJECT models/button_menu_item_model.cc models/combobox_model.cc models/dialog_model.cc models/list_selection_model.cc models/menu_model.cc models/simple_combobox_model.cc models/simple_menu_model.cc models/table_model.cc models/tree_model.cc)

# resource
ADD_LIBRARY(resource OBJECT resource/data_pack.cc resource/material_design/material_design_controller.cc resource/resource_bundle.cc resource/resource_bundle_auralinux.cc)

# touch
ADD_LIBRARY(touch OBJECT touch/selection_bound.cc touch/touch_device_linux.cc touch/touch_editing_controller.cc touch/touch_enabled.cc)

# ime
ADD_LIBRARY(ime OBJECT ime/candidate_window.cc ime/composition_text.cc ime/ime_bridge.cc ime/ime_engine_handler_interface.cc ime/infolist_entry.cc ime/input_method_auralinux.cc ime/input_method_base.cc ime/input_method_factory.cc ime/input_method_initializer.cc ime/input_method_minimal.cc ime/mock_input_method.cc ime/text_input_client.cc ime/linux/fake_input_method_context.cc ime/linux/fake_input_method_context_factory.cc ime/linux/linux_input_method_context_factory.cc)

# backend
ADD_LIBRARY(backend OBJECT ${BACKEND_SOURCE_FILES})


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_SOURCE_DIR}/include/core ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/.. ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${backend_LIBRARIES} -ldl)

ADD_LIBRARY(ui_base SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:accelerators> $<TARGET_OBJECTS:clipboard> $<TARGET_OBJECTS:cursor> $<TARGET_OBJECTS:dragdrop> $<TARGET_OBJECTS:l10n> $<TARGET_OBJECTS:models> $<TARGET_OBJECTS:resource> $<TARGET_OBJECTS:touch> $<TARGET_OBJECTS:ime> $<TARGET_OBJECTS:backend>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_base ${link_libraries})

INSTALL(TARGETS ui_base
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
