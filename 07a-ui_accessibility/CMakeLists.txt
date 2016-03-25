PROJECT(chromium-ui-accessibility CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Accessibility")

# Lotation of required tools
SET(JSON_COMPILER_DIR "${CMAKE_SOURCE_DIR}/../../tools/json_schema_compiler")

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
PKG_CHECK_MODULES(gconf REQUIRED gconf-2.0)
PKG_CHECK_MODULES(atk REQUIRED atk)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DACCESSIBILITY_IMPLEMENTATION -DOS_LINUX -DUSE_AURA -DATK_LIB_DIR="${CMAKE_INSTALL_LIBDIR}")

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .IDL -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .idl...")

EXECUTE_PROCESS(COMMAND ${JSON_COMPILER_DIR}/compiler.py ax_enums.idl --root=../.. --destdir=${CMAKE_BINARY_DIR} --namespace=ui --generator=cpp --include-rules= WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST

# ax
ADD_LIBRARY(ax OBJECT ax_node.cc ax_node_data.cc ax_serializable_tree.cc ax_text_utils.cc ax_tree.cc ax_tree_data.cc ax_tree_serializer.cc ax_view_state.cc)

# platform
ADD_LIBRARY(platform OBJECT platform/ax_platform_node.cc platform/ax_platform_node_base.cc platform/ax_platform_node_auralinux.cc platform/atk_util_auralinux.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/accessibility/ax_enums.cc)


# all files refer to "ui/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${gconf_INCLUDE_DIRS} ${atk_INCLUDE_DIRS} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/../..)
SET(link_libraries ${LINK_LIBRARIES} ${gconf_LIBRARIES} ${atk_LIBRARIES})

ADD_LIBRARY(ui_accessibility SHARED $<TARGET_OBJECTS:ax> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_accessibility ${link_libraries})

INSTALL(TARGETS ui_accessibility
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
