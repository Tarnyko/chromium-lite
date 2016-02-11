PROJECT(chromium-mojo CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Mojo")

# CMake modules
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DMOJO_ENVIRONMENT_IMPL_IMPLEMENTATION -DMOJO_MESSAGE_PUMP_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# environment
ADD_LIBRARY(environment OBJECT public/cpp/environment/lib/async_waiter.cc public/cpp/environment/lib/logging.cc public/cpp/environment/lib/scoped_task_tracking.cc environment/default_async_waiter_impl.cc environment/default_logger_impl.cc environment/default_run_loop_impl.cc environment/default_task_tracker_impl.cc environment/environment.cc)

# message_pump
ADD_LIBRARY(message_pump OBJECT message_pump/handle_watcher.cc message_pump/time_helper.cc message_pump/message_pump_mojo.cc)


# all files refer to "mojo/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)

ADD_LIBRARY(mojo SHARED $<TARGET_OBJECTS:environment> $<TARGET_OBJECTS:message_pump>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(mojo ${link_libraries})

INSTALL(TARGETS mojo
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
