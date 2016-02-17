PROJECT(chromium-ui-views CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Views")

# Location of direct dependencies
SET(NATIVE_THEME_DIR "${CMAKE_SOURCE_DIR}/../native_theme")
SET(TOUCH_SELECTION_DIR "${CMAKE_SOURCE_DIR}/../touch_selection")
SET(DISPLAY_UTIL_DIR "${CMAKE_SOURCE_DIR}/../display/util")
SET(UI_ACCESSIBILITY_DIR "${CMAKE_SOURCE_DIR}/../accessibility")
SET(UI_GFX_DIR "${CMAKE_SOURCE_DIR}/../gfx")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/../..")

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
FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  SET(BACKEND_SOURCE_FILES widget/desktop_aura/desktop_factory_ozone.cc widget/desktop_aura/desktop_screen_ozone.cc widget/desktop_aura/desktop_window_tree_host_ozone.cc)
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  PKG_CHECK_MODULES(backend REQUIRED x11 xext xrandr atk)
  SET(BACKEND_SOURCE_FILES accessibility/native_view_accessibility_auralinux.cc widget/desktop_aura/desktop_drag_drop_client_aurax11.cc widget/desktop_aura/desktop_screen_x11.cc widget/desktop_aura/desktop_window_tree_host_x11.cc widget/desktop_aura/x11_desktop_handler.cc widget/desktop_aura/x11_desktop_window_move_client.cc widget/desktop_aura/x11_pointer_grab.cc widget/desktop_aura/x11_topmost_window_finder.cc widget/desktop_aura/x11_whole_screen_move_loop.cc widget/desktop_aura/x11_window_event_filter.cc ${DISPLAY_UTIL_DIR}/display_util.cc ${DISPLAY_UTIL_DIR}/edid_parser.cc ${DISPLAY_UTIL_DIR}/x11/edid_parser_x11.cc)
  ADD_DEFINITIONS(-DUSE_X11 -DDISPLAY_UTIL_IMPLEMENTATION)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DVIEWS_IMPLEMENTATION -DNATIVE_THEME_IMPLEMENTATION -DUI_TOUCH_SELECTION_IMPLEMENTATION -DTOOLKIT_VIEWS=1 -DUSE_AURA)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .GRD -> .CC,.H,.PAK

MESSAGE(STATUS "Generating .cc,.h,.pak from .grd...")

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i views_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o . --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/resources)


# SOURCES FILE LIST

# native_theme
ADD_LIBRARY(native_theme OBJECT ${NATIVE_THEME_DIR}/common_theme.cc ${NATIVE_THEME_DIR}/native_theme.cc ${NATIVE_THEME_DIR}/native_theme_base.cc ${NATIVE_THEME_DIR}/native_theme_aura.cc ${NATIVE_THEME_DIR}/native_theme_dark_aura.cc ${NATIVE_THEME_DIR}/native_theme_observer.cc ${NATIVE_THEME_DIR}/native_theme_switches.cc)

# touch_selection
ADD_LIBRARY(touch_selection OBJECT ${TOUCH_SELECTION_DIR}/longpress_drag_selector.cc ${TOUCH_SELECTION_DIR}/touch_handle.cc ${TOUCH_SELECTION_DIR}/touch_handle_drawable_aura.cc ${TOUCH_SELECTION_DIR}/touch_selection_controller.cc ${TOUCH_SELECTION_DIR}/touch_selection_menu_runner.cc)

# accessibility
ADD_LIBRARY(accessibility OBJECT accessibility/native_view_accessibility.cc accessibility/ax_aura_obj_cache.cc accessibility/ax_view_obj_wrapper.cc accessibility/ax_widget_obj_wrapper.cc accessibility/ax_window_obj_wrapper.cc)

# animation
ADD_LIBRARY(animation OBJECT animation/bounds_animator.cc animation/button_ink_drop_delegate.cc animation/ink_drop_animation.cc animation/ink_drop_animation_controller_factory.cc animation/ink_drop_animation_controller_impl.cc animation/ink_drop_animation_observer.cc animation/ink_drop_painted_layer_delegates.cc animation/ink_drop_state.cc animation/scroll_animator.cc)

# bubble
ADD_LIBRARY(bubble OBJECT bubble/bubble_window_targeter.cc bubble/bubble_border.cc bubble/bubble_delegate.cc bubble/bubble_frame_view.cc bubble/tray_bubble_view.cc)

# controls
ADD_LIBRARY(controls OBJECT controls/button/blue_button.cc controls/button/button.cc controls/button/checkbox.cc controls/button/custom_button.cc controls/button/image_button.cc controls/button/label_button.cc controls/button/label_button_border.cc controls/button/menu_button.cc controls/button/radio_button.cc controls/combobox/combobox.cc controls/focusable_border.cc controls/glow_hover_controller.cc controls/image_view.cc controls/label.cc controls/link.cc controls/menu/display_change_listener_aura.cc controls/menu/menu_config.cc controls/menu/menu_config_linux.cc controls/menu/menu_controller.cc controls/menu/menu_delegate.cc controls/menu/menu_event_dispatcher.cc controls/menu/menu_key_event_handler.cc controls/menu/menu_message_loop_aura.cc controls/menu/menu_host.cc controls/menu/menu_host_root_view.cc controls/menu/menu_image_util.cc controls/menu/menu_item_view.cc controls/menu/menu_listener.cc controls/menu/menu_model_adapter.cc controls/menu/menu_runner.cc controls/menu/menu_runner_impl.cc controls/menu/menu_runner_impl_adapter.cc controls/menu/menu_scroll_view_container.cc controls/menu/menu_separator_views.cc controls/menu/submenu_view.cc controls/message_box_view.cc controls/native/native_view_host.cc controls/native/native_view_host_aura.cc controls/prefix_selector.cc controls/progress_bar.cc controls/resize_area.cc controls/scroll_view.cc controls/scrollbar/base_scroll_bar.cc controls/scrollbar/base_scroll_bar_button.cc controls/scrollbar/base_scroll_bar_thumb.cc controls/scrollbar/kennedy_scroll_bar.cc controls/scrollbar/native_scroll_bar.cc controls/scrollbar/native_scroll_bar_views.cc controls/scrollbar/overlay_scroll_bar.cc controls/scrollbar/scroll_bar.cc controls/separator.cc controls/single_split_view.cc controls/slide_out_view.cc controls/slider.cc controls/styled_label.cc controls/tabbed_pane/tabbed_pane.cc controls/table/table_header.cc controls/table/table_utils.cc controls/table/table_view.cc controls/textfield/textfield.cc controls/textfield/textfield_controller.cc controls/textfield/textfield_model.cc controls/throbber.cc controls/tree/tree_view.cc controls/tree/tree_view_controller.cc)

# corewm
ADD_LIBRARY(corewm OBJECT corewm/tooltip_aura.cc corewm/tooltip_controller.cc)

# focus
ADD_LIBRARY(focus OBJECT focus/external_focus_tracker.cc focus/focus_manager.cc focus/focus_manager_factory.cc focus/focus_search.cc focus/view_storage.cc focus/widget_focus_manager.cc)

# layout
ADD_LIBRARY(layout OBJECT layout/box_layout.cc layout/fill_layout.cc layout/grid_layout.cc layout/layout_manager.cc)

# linuxui
ADD_LIBRARY(linuxui OBJECT linux_ui/linux_ui.cc linux_ui/status_icon_linux.cc linux_ui/window_button_order_provider.cc)

# touchui
ADD_LIBRARY(touchui OBJECT touchui/touch_selection_controller_impl.cc touchui/touch_selection_menu_runner_views.cc)

# widget
ADD_LIBRARY(widget OBJECT widget/drop_helper.cc widget/native_widget_aura.cc widget/root_view.cc widget/root_view_targeter.cc widget/tooltip_manager.cc widget/tooltip_manager_aura.cc widget/widget.cc widget/widget_aura_utils.cc widget/widget_delegate.cc widget/widget_deletion_observer.cc widget/window_reorderer.cc widget/desktop_aura/desktop_capture_client.cc widget/desktop_aura/desktop_cursor_loader_updater_auralinux.cc widget/desktop_aura/desktop_dispatcher_client.cc widget/desktop_aura/desktop_event_client.cc widget/desktop_aura/desktop_focus_rules.cc widget/desktop_aura/desktop_native_cursor_manager.cc widget/desktop_aura/desktop_native_widget_aura.cc widget/desktop_aura/desktop_screen_position_client.cc)

# window
ADD_LIBRARY(window OBJECT window/client_view.cc window/custom_frame_view.cc window/dialog_client_view.cc window/dialog_delegate.cc window/frame_background.cc window/native_frame_view.cc window/non_client_view.cc window/window_shape.cc)

# misc
ADD_LIBRARY(misc OBJECT accessible_pane_view.cc background.cc border.cc button_drag_utils.cc color_chooser/color_chooser_view.cc debug_utils.cc drag_utils.cc drag_utils_aura.cc event_monitor_aura.cc masked_targeter_delegate.cc metrics.cc metrics_aura.cc mouse_watcher.cc mouse_watcher_view_host.cc native_cursor_aura.cc painter.cc rect_based_targeting_utils.cc repeat_controller.cc round_rect_painter.cc shadow_border.cc style/platform_style.cc style/platform_style_linux.cc view.cc view_constants.cc view_constants_aura.cc view_model.cc view_model_utils.cc view_targeter.cc view_targeter_delegate.cc views_delegate.cc views_switches.cc views_touch_selection_controller_factory_aura.cc)


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/../.. ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${UI_ACCESSIBILITY_DIR}/Build ${UI_GFX_DIR}/Build)
# TODO : "${UI_*_DIR}/Build" are HACKs ! remove them when use .pc files !
SET(link_libraries ${backend_LIBRARIES})

ADD_LIBRARY(ui_views SHARED $<TARGET_OBJECTS:native_theme> $<TARGET_OBJECTS:touch_selection> $<TARGET_OBJECTS:accessibility> $<TARGET_OBJECTS:animation> $<TARGET_OBJECTS:bubble> $<TARGET_OBJECTS:controls> $<TARGET_OBJECTS:corewm> $<TARGET_OBJECTS:focus> $<TARGET_OBJECTS:layout> $<TARGET_OBJECTS:linuxui> $<TARGET_OBJECTS:touchui> $<TARGET_OBJECTS:widget> $<TARGET_OBJECTS:window> $<TARGET_OBJECTS:misc> ${BACKEND_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_views ${link_libraries})

INSTALL(TARGETS ui_views
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
