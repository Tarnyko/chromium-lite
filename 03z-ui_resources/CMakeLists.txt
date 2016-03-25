PROJECT(chromium-ui-resources CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - UI Resources")

# Variable paths for required third-parties
SET(GRIT_DIR "${CMAKE_SOURCE_DIR}/../..")

# CMake modules
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Common definitions
IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


MESSAGE(STATUS "Generating .pak and .cc/.h from .grd...")

EXECUTE_PROCESS(COMMAND mkdir -p ui/resources/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ui_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/ui/resources --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ui_unscaled_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/ui/resources --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i ../webui/resources/webui_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR}/ui/resources --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/ui/resources/grit/ui_resources_map.cc ${CMAKE_BINARY_DIR}/ui/resources/grit/webui_resources_map.cc)

SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/ui/resources ${CMAKE_SOURCE_DIR}/../..)

ADD_LIBRARY(ui_resources SHARED $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(ui_resources ${link_libraries})

INSTALL(TARGETS ui_resources
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
