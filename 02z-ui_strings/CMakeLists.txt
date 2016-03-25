PROJECT(chromium-ui-strings CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Strings")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/../..")

# Mandatory dependencies
FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)


MESSAGE(STATUS "Generating .pak and .h from .grd...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/strings/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ui_strings.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/ui/strings --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i app_locale_settings.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/ui/strings --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

