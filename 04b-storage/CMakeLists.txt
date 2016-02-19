PROJECT(chromium-storage CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Storage")

# re2/snappy source path, needed for some headers by leveldb Chromium extensions
SET(RE2_SRC_DIR "${CMAKE_SOURCE_DIR}/../third_party/re2")
SET(SNAPPY_SRC_DIR "${CMAKE_SOURCE_DIR}/../third_party/snappy")

# leveldb fork path, needed for some headers and Chromium extensions
SET(LEVELDB_EXT_DIR "${CMAKE_SOURCE_DIR}/../third_party/leveldatabase")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(leveldb REQUIRED leveldb)

# Common definitions
ADD_DEFINITIONS(-DSTORAGE_COMMON_IMPLEMENTATION -DSTORAGE_BROWSER_IMPLEMENTATION -DLEVELDB_PLATFORM_CHROMIUM)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# SOURCES FILE LIST

# common
ADD_LIBRARY(common OBJECT common/data_element.cc common/blob_storage/blob_item_bytes_request.cc common/blob_storage/blob_item_bytes_response.cc common/database/database_connections.cc common/database/database_identifier.cc common/fileapi/directory_entry.cc common/fileapi/file_system_info.cc common/fileapi/file_system_util.cc common/quota/quota_status_code.cc)

# browser_blob
ADD_LIBRARY(browser_blob OBJECT browser/blob/blob_async_builder_host.cc browser/blob/blob_async_transport_strategy.cc browser/blob/blob_data_builder.cc browser/blob/blob_data_handle.cc browser/blob/blob_data_item.cc browser/blob/blob_data_snapshot.cc browser/blob/blob_reader.cc browser/blob/blob_storage_context.cc browser/blob/blob_storage_registry.cc browser/blob/blob_url_request_job.cc browser/blob/blob_url_request_job_factory.cc browser/blob/internal_blob_data.cc browser/blob/scoped_file.cc browser/blob/shareable_blob_data_item.cc browser/blob/shareable_file_reference.cc browser/blob/upload_blob_element_reader.cc browser/blob/view_blob_internals_job.cc)

# browser_database
ADD_LIBRARY(browser_database OBJECT browser/database/database_quota_client.cc browser/database/database_tracker.cc browser/database/database_util.cc browser/database/databases_table.cc browser/database/vfs_backend.cc)

# browser_fileapi
ADD_LIBRARY(browser_fileapi OBJECT browser/fileapi/async_file_util_adapter.cc browser/fileapi/copy_or_move_operation_delegate.cc browser/fileapi/dragged_file_util.cc browser/fileapi/dump_file_system.cc browser/fileapi/external_mount_points.cc browser/fileapi/file_stream_reader.cc browser/fileapi/file_system_context.cc browser/fileapi/file_system_dir_url_request_job.cc browser/fileapi/file_system_file_stream_reader.cc browser/fileapi/file_system_file_util.cc browser/fileapi/file_system_operation_context.cc browser/fileapi/file_system_operation_impl.cc browser/fileapi/file_system_operation_runner.cc browser/fileapi/file_system_options.cc browser/fileapi/file_system_quota_client.cc browser/fileapi/file_system_url.cc browser/fileapi/file_system_url_request_job.cc browser/fileapi/file_system_url_request_job_factory.cc browser/fileapi/file_system_usage_cache.cc browser/fileapi/file_writer_delegate.cc browser/fileapi/isolated_context.cc browser/fileapi/isolated_file_system_backend.cc browser/fileapi/local_file_stream_reader.cc browser/fileapi/local_file_stream_writer.cc browser/fileapi/local_file_util.cc browser/fileapi/mount_points.cc browser/fileapi/native_file_util.cc browser/fileapi/obfuscated_file_util.cc browser/fileapi/plugin_private_file_system_backend.cc browser/fileapi/quota/open_file_handle.cc browser/fileapi/quota/open_file_handle_context.cc browser/fileapi/quota/quota_backend_impl.cc browser/fileapi/quota/quota_reservation.cc browser/fileapi/quota/quota_reservation.cc browser/fileapi/quota/quota_reservation_buffer.cc browser/fileapi/quota/quota_reservation_manager.cc browser/fileapi/recursive_operation_delegate.cc browser/fileapi/remove_operation_delegate.cc browser/fileapi/sandbox_directory_database.cc browser/fileapi/sandbox_file_stream_writer.cc browser/fileapi/sandbox_file_system_backend.cc browser/fileapi/sandbox_file_system_backend_delegate.cc browser/fileapi/sandbox_isolated_origin_database.cc browser/fileapi/sandbox_origin_database.cc browser/fileapi/sandbox_origin_database_interface.cc browser/fileapi/sandbox_prioritized_origin_database.cc browser/fileapi/sandbox_quota_observer.cc browser/fileapi/timed_task_helper.cc browser/fileapi/transient_file_util.cc)

# browser_quota
ADD_LIBRARY(browser_quota OBJECT browser/quota/client_usage_tracker.cc browser/quota/quota_database.cc browser/quota/quota_manager.cc browser/quota/quota_manager_proxy.cc browser/quota/quota_task.cc browser/quota/quota_temporary_storage_evictor.cc browser/quota/special_storage_policy.cc browser/quota/storage_monitor.cc browser/quota/storage_observer.cc browser/quota/usage_tracker.cc)

# re2 (required by "leveldb_ext" below)
ADD_LIBRARY(re2 OBJECT ${RE2_SRC_DIR}/re2/bitstate.cc ${RE2_SRC_DIR}/re2/compile.cc ${RE2_SRC_DIR}/re2/dfa.cc ${RE2_SRC_DIR}/re2/filtered_re2.cc ${RE2_SRC_DIR}/re2/mimics_pcre.cc ${RE2_SRC_DIR}/re2/nfa.cc ${RE2_SRC_DIR}/re2/onepass.cc ${RE2_SRC_DIR}/re2/parse.cc ${RE2_SRC_DIR}/re2/perl_groups.cc ${RE2_SRC_DIR}/re2/prefilter.cc ${RE2_SRC_DIR}/re2/prefilter_tree.cc ${RE2_SRC_DIR}/re2/prog.cc ${RE2_SRC_DIR}/re2/re2.cc ${RE2_SRC_DIR}/re2/regexp.cc ${RE2_SRC_DIR}/re2/set.cc ${RE2_SRC_DIR}/re2/simplify.cc ${RE2_SRC_DIR}/re2/stringpiece.cc ${RE2_SRC_DIR}/re2/tostring.cc ${RE2_SRC_DIR}/re2/unicode_casefold.cc ${RE2_SRC_DIR}/re2/unicode_groups.cc ${RE2_SRC_DIR}/util/hash.cc ${RE2_SRC_DIR}/util/logging.cc ${RE2_SRC_DIR}/util/rune.cc ${RE2_SRC_DIR}/util/stringprintf.cc ${RE2_SRC_DIR}/util/strutil.cc ${RE2_SRC_DIR}/util/valgrind.cc)

# leveldb_ext (these are Chromium-specific !)
ADD_LIBRARY(leveldb_ext OBJECT ${LEVELDB_EXT_DIR}/src/helpers/memenv/memenv.cc ${LEVELDB_EXT_DIR}/port/port_chromium.cc ${LEVELDB_EXT_DIR}/env_chromium.cc)


# all files refer to "storage/...", so we include ".." here
# we put LEVELDB_EXT_DIR first, because the fork has specific required headers/values
SET(include_dirs ${INCLUDE_DIRS} ${RE2_SRC_DIR} ${SNAPPY_SRC_DIR}/linux ${LEVELDB_EXT_DIR} ${LEVELDB_EXT_DIR}/src ${LEVELDB_EXT_DIR}/src/include ${leveldb_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${leveldb_LIBRARIES})

ADD_LIBRARY(storage SHARED $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:browser_blob> $<TARGET_OBJECTS:browser_database> $<TARGET_OBJECTS:browser_fileapi> $<TARGET_OBJECTS:browser_quota> $<TARGET_OBJECTS:re2> $<TARGET_OBJECTS:leveldb_ext>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(storage ${link_libraries})

INSTALL(TARGETS storage
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
