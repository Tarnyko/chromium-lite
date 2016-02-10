PROJECT(chromium-gin CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Gin")

# CMake modules
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DGIN_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT arguments.cc array_buffer.cc context_holder.cc converter.cc debug_impl.cc dictionary.cc function_template.cc interceptor.cc isolate_holder.cc modules/console.cc modules/file_module_provider.cc modules/module_registry.cc modules/module_runner_delegate.cc modules/timer.cc object_template_builder.cc per_context_data.cc per_isolate_data.cc runner.cc run_microtasks_observer.cc shell_runner.cc try_catch.cc v8_initializer.cc v8_isolate_memory_dump_provider.cc v8_platform.cc wrappable.cc wrapper_info.cc)


# all files refer to "gin/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)

ADD_LIBRARY(gin SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(gin ${link_libraries})

INSTALL(TARGETS gin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
