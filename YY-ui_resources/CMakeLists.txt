PROJECT(chromium-ui-resources CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Resources")

# Variable paths for required third-parties
SET(GRIT_DIR "../..")

# CMake modules
INCLUDE(FindPythonLibs)

# Mandatory dependencies
IF(NOT PYTHONLIBS_FOUND)
  MESSAGE(FATAL_ERROR "Python is required to generate some source files !")
ENDIF(NOT PYTHONLIBS_FOUND)


MESSAGE(STATUS "Generating .pak from .grd...")

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ui_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o . --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ui_unscaled_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o . --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ../webui/resources/webui_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o . --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

