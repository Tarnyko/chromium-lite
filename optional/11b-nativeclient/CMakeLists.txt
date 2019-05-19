PROJECT(chromium-nativeclient C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Native Client")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Common definitions
ADD_DEFINITIONS(-DOS_LINUX -D_POSIX_C_SOURCE=199506 -D_XOPEN_SOURCE=600 -D_GNU_SOURCE=1 -D__STDC_LIMIT_MACROS=1)

IF(CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-std=c99)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# gio
ADD_LIBRARY(gio OBJECT src/shared/gio/gio.c src/shared/gio/gprintf.c)

# imc
ADD_LIBRARY(imc OBJECT src/shared/imc/nacl_imc_common.cc src/shared/imc/linux/nacl_imc.cc src/shared/imc/posix/nacl_imc_posix.cc)

# platform
ADD_LIBRARY(platform OBJECT src/shared/platform/nacl_check.c src/shared/platform/nacl_global_secure_random.c src/shared/platform/nacl_host_desc_common.c src/shared/platform/nacl_interruptible_condvar.c src/shared/platform/nacl_interruptible_mutex.c src/shared/platform/nacl_log.c src/shared/platform/nacl_secure_random_common.c src/shared/platform/nacl_sync_checked.c src/shared/platform/nacl_time_common.c src/shared/platform/platform_init.c src/shared/platform/refcount_base.cc src/shared/platform/posix/aligned_malloc.c src/shared/platform/posix/condition_variable.c src/shared/platform/posix/lock.c src/shared/platform/posix/nacl_error.c src/shared/platform/posix/nacl_exit.c src/shared/platform/posix/nacl_fast_mutex.c src/shared/platform/posix/nacl_file_lock.c src/shared/platform/posix/nacl_find_addrsp.c src/shared/platform/posix/nacl_host_desc.c src/shared/platform/posix/nacl_secure_random.c src/shared/platform/posix/nacl_thread_id.c src/shared/platform/posix/nacl_threads.c src/shared/platform/posix/nacl_time.c src/shared/platform/posix/nacl_timestamp.c src/shared/platform/linux/nacl_clock.c src/shared/platform/linux/nacl_host_dir.c src/shared/platform/linux/nacl_semaphore.c)

# elfloader
ADD_LIBRARY(elfloader OBJECT src/untrusted/elf_loader/elf_loader.c)

# nacl
ADD_LIBRARY(nacl OBJECT src/untrusted/nacl/nacl_irt.c src/untrusted/nacl/nacl_interface_query.c)


# all files refer to "ui/...", so we include "../.." here
SET(include_dirs ${INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${LINK_LIBRARIES})

ADD_LIBRARY(nativeclient SHARED $<TARGET_OBJECTS:gio> $<TARGET_OBJECTS:imc> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:elfloader> $<TARGET_OBJECTS:nacl>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(nativeclient ${link_libraries})

INSTALL(TARGETS nativeclient
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
