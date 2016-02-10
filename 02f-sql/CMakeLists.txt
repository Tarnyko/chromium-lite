PROJECT(chromium-sql CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - SQL")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(sqlite REQUIRED sqlite3)

# Common definitions
ADD_DEFINITIONS(-DSQL_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

ADD_LIBRARY(src OBJECT connection.cc error_delegate_util.cc meta_table.cc recovery.cc sql_memory_dump_provider.cc statement.cc transaction.cc)


# all files refer to "sql/...", so we include ".." here
SET(include_dirs ${INCLUDE_DIRS} ${sqlite_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${sqlite_LIBRARIES})

ADD_LIBRARY(sql SHARED $<TARGET_OBJECTS:src>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(sql ${link_libraries})

INSTALL(TARGETS sql
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
