PROJECT(chromium-base C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Base")

# Location of third-party dependencies
# (libicu fork is required by "i18n" sources)
SET(ICU_FORK_DIR "${CMAKE_SOURCE_DIR}/../third_party/icu")
SET(MODP_DIR "${CMAKE_SOURCE_DIR}/../third_party/modp_b64")
SET(CED_DIR "${CMAKE_SOURCE_DIR}/../third_party/ced")

# Location of build tools dir (required for generating "generated_build_date.h", "features.h" and "debugging_flags.h")
SET(BUILD_TOOLS_DIR "${CMAKE_SOURCE_DIR}/../build")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindThreads)
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PACKAGE(Threads)
PKG_CHECK_MODULES(libevent REQUIRED libevent)
PKG_CHECK_MODULES(icui18n REQUIRED icu-i18n >= 53.0)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Optional dependencies and definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DOS_POSIX=1 -DCOMPONENT_BUILD=1 -DBASE_IMPLEMENTATION=1 -DNO_TCMALLOC -D_FILE_OFFSET_BITS=64 -D__STDC_FORMAT_MACROS)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION GENERATED_BUILD_DATE.H (NEEDED BY "build_time.cc")

EXECUTE_PROCESS(COMMAND sh -c "python ${BUILD_TOOLS_DIR}/write_build_date_header.py generated_build_date.h default \"`LANG=C date '+%b %d %Y %H:%M:%S'`\"" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# PRE-GENERATION FEATURES.H (NEEDED BY "tracked_objects.h")

MESSAGE(STATUS "Generating features.h...")

EXECUTE_PROCESS(COMMAND mkdir -p base/allocator WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND sh -c "echo --flags USE_EXPERIMENTAL_ALLOCATOR_SHIM=0 > buildflag_header.rsp" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND python ${BUILD_TOOLS_DIR}/write_buildflag_header.py --output base/allocator/features.h --rulename //base/allocator:features --gen-dir ${CMAKE_BINARY_DIR} --definitions ${CMAKE_BINARY_DIR}/buildflag_header.rsp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# PRE-GENERATION DEBUGGING_FLAGS.H (NEEDED BY "debug/profiler.cc")

MESSAGE(STATUS "Generating debugging_flags.h...")

EXECUTE_PROCESS(COMMAND mkdir -p base/debug WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND sh -c "echo --flags ENABLE_PROFILING=0 ENABLE_MEMORY_TASK_PROFILER=1 > buildflag_header.rsp" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND python ${BUILD_TOOLS_DIR}/write_buildflag_header.py --output base/debug/debugging_flags.h --rulename _base_debugging_flags --gen-dir ${CMAKE_BINARY_DIR} --definitions ${CMAKE_BINARY_DIR}/buildflag_header.rsp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST, PER SECTION

# switches
ADD_LIBRARY(switches OBJECT base_switches.cc)

# command_line
ADD_LIBRARY(command_line OBJECT command_line.cc)

# strings
ADD_LIBRARY(strings OBJECT third_party/icu/icu_utf.cc third_party/dmg_fp/g_fmt.cc third_party/dmg_fp/dtoa_wrapper.cc strings/utf_string_conversion_utils.cc strings/utf_string_conversions.cc strings/utf_offset_string_conversions.cc strings/latin1_string_conversions.cc strings/string16.cc strings/string_piece.cc strings/string_util.cc strings/string_util_constants.cc strings/string_split.cc strings/stringprintf.cc strings/safe_sprintf.cc strings/sys_string_conversions_posix.cc strings/string_number_conversions.cc strings/pattern.cc json/string_escape.cc json/json_writer.cc json/json_reader.cc json/json_parser.cc json/json_file_value_serializer.cc json/json_string_value_serializer.cc json/json_value_converter.cc)

# logging
ADD_LIBRARY(logging OBJECT logging.cc trace_event/trace_log.cc trace_event/trace_log_constants.cc trace_event/trace_config.cc trace_event/trace_buffer.cc trace_event/trace_event_impl.cc trace_event/tracing_agent.cc trace_event/malloc_dump_provider.cc trace_event/trace_event_synthetic_delay.cc trace_event/trace_event_system_stats_monitor.cc trace_event/trace_event_memory_overhead.cc trace_event/memory_dump_manager.cc trace_event/memory_dump_request_args.cc trace_event/process_memory_dump.cc trace_event/memory_dump_session_state.cc trace_event/memory_allocator_dump.cc trace_event/memory_allocator_dump_guid.cc trace_event/memory_usage_estimator.cc trace_event/process_memory_maps.cc trace_event/process_memory_totals.cc trace_event/heap_profiler_allocation_register.cc trace_event/heap_profiler_allocation_register_posix.cc trace_event/heap_profiler_heap_dump_writer.cc trace_event/heap_profiler_stack_frame_deduplicator.cc trace_event/heap_profiler_type_name_deduplicator.cc trace_event/heap_profiler_allocation_context.cc trace_event/heap_profiler_allocation_context_tracker.cc trace_event/heap_profiler_event_filter.cc trace_event/memory_infra_background_whitelist.cc trace_event/category_registry.cc trace_event/event_name_filter.cc trace_event/trace_event_filter.cc trace_event/trace_event_argument.cc)

# i18n
ADD_LIBRARY(i18n OBJECT i18n/base_i18n_switches.cc i18n/bidi_line_iterator.cc i18n/break_iterator.cc i18n/build_utf8_validator_tables.cc i18n/case_conversion.cc i18n/char_iterator.cc i18n/file_util_icu.cc i18n/i18n_constants.cc i18n/icu_string_conversions.cc i18n/icu_util.cc i18n/message_formatter.cc i18n/number_formatting.cc i18n/rtl.cc i18n/streaming_utf8_validator.cc i18n/string_compare.cc i18n/string_search.cc i18n/time_formatting.cc i18n/timezone.cc i18n/utf8_validator_tables.cc i18n/encoding_detection.cc ${CED_DIR}/src/util/encodings/encodings.cc ${CED_DIR}/src/util/languages/languages.cc ${CED_DIR}/src/compact_enc_det/compact_enc_det.cc ${CED_DIR}/src/compact_enc_det/compact_enc_det_hint_code.cc)
# - "i18n/message_formatter.cc" requires the "unicode/uformattable.h" header,
# which only exists in the "third_party/icu" fork
# - "i18n/encodind_detection.cc" requires sources from "third_party/ced" fork

# debug
ADD_LIBRARY(debug OBJECT debug/debugger.cc debug/debugger_posix.cc debug/profiler.cc debug/crash_logging.cc debug/dump_without_crashing.cc debug/stack_trace.cc debug/stack_trace_posix.cc debug/task_annotator.cc debug/alias.cc debug/activity_tracker.cc debug/thread_heap_usage_tracker.cc)

# lock
ADD_LIBRARY(lock OBJECT synchronization/lock.cc synchronization/lock_impl_posix.cc synchronization/read_write_lock_posix.cc synchronization/waitable_event_posix.cc synchronization/waitable_event_watcher_posix.cc synchronization/condition_variable_posix.cc synchronization/atomic_flag.cc)

# memory
ADD_LIBRARY(memory OBJECT memory/singleton.cc memory/aligned_memory.cc memory/discardable_memory.cc memory/discardable_shared_memory.cc memory/discardable_memory_allocator.cc memory/ref_counted.cc memory/ref_counted_memory.cc memory/shared_memory_helper.cc memory/shared_memory_posix.cc memory/weak_ptr.cc memory/memory_coordinator_client_registry.cc memory/memory_pressure_listener.cc memory/memory_pressure_monitor.cc allocator/allocator_extension.cc)

# message_loop
ADD_LIBRARY(messageloop OBJECT message_loop/message_loop.cc message_loop/message_loop_task_runner.cc message_loop/message_pump.cc message_loop/message_pump_default.cc message_loop/message_pump_libevent.cc message_loop/incoming_task_queue.cc run_loop.cc)

# native_library
ADD_LIBRARY(nativelibrary OBJECT native_library.cc native_library_posix.cc scoped_native_library.cc)

# version
ADD_LIBRARY(version OBJECT version.cc)

# vlog
ADD_LIBRARY(vlog OBJECT vlog.cc)

# pickle
ADD_LIBRARY(pickle OBJECT pickle.cc)

# base_paths
ADD_LIBRARY(paths OBJECT base_paths.cc base_paths_posix.cc)

# at_exit
ADD_LIBRARY(atexit OBJECT at_exit.cc)

# callback
ADD_LIBRARY(callback OBJECT callback_internal.cc callback_helpers.cc bind_helpers.cc)

# lazy_instance
ADD_LIBRARY(lazyinstance OBJECT lazy_instance.cc)

# posix
ADD_LIBRARY(posix OBJECT posix/file_descriptor_shuffle.cc posix/global_descriptors.cc posix/safe_strerror.cc posix/unix_domain_socket_linux.cc)

# sys_info
ADD_LIBRARY(sysinfo OBJECT sys_info.cc sys_info_posix.cc sys_info_linux.cc cpu.cc)

# system_monitor
ADD_LIBRARY(systemmonitor OBJECT system_monitor/system_monitor.cc)

# task_scheduler
ADD_LIBRARY(taskscheduler OBJECT task_scheduler/task.cc task_scheduler/task_scheduler.cc task_scheduler/task_scheduler_impl.cc task_scheduler/task_traits.cc task_scheduler/task_tracker.cc task_scheduler/task_tracker_posix.cc task_scheduler/sequence.cc task_scheduler/sequence_sort_key.cc task_scheduler/priority_queue.cc task_scheduler/scheduler_worker.cc task_scheduler/scheduler_worker_stack.cc task_scheduler/scheduler_worker_pool_impl.cc task_scheduler/scheduler_worker_pool_params.cc task_scheduler/scheduler_lock_impl.cc task_scheduler/scoped_set_task_priority_for_current_thread.cc task_scheduler/delayed_task_manager.cc task_scheduler/post_task.cc)

# threading
ADD_LIBRARY(threading OBJECT threading/thread.cc threading/simple_thread.cc threading/platform_thread_linux.cc threading/platform_thread_posix.cc threading/platform_thread_internal_posix.cc threading/thread_local_storage.cc threading/thread_local_storage_posix.cc threading/thread_collision_warner.cc threading/thread_restrictions.cc threading/thread_id_name_manager.cc threading/thread_checker_impl.cc threading/thread_task_runner_handle.cc threading/worker_pool.cc threading/worker_pool_posix.cc threading/sequenced_worker_pool.cc threading/non_thread_safe_impl.cc threading/post_task_and_reply_impl.cc threading/sequenced_task_runner_handle.cc sequence_checker_impl.cc task_runner.cc sequenced_task_runner.cc pending_task.cc)

# xdg
ADD_LIBRARY(xdg OBJECT nix/mime_util_xdg.cc nix/xdg_util.cc third_party/xdg_mime/xdgmime.c third_party/xdg_mime/xdgmimealias.c third_party/xdg_mime/xdgmimecache.c third_party/xdg_mime/xdgmimeglob.c third_party/xdg_mime/xdgmimeicon.c third_party/xdg_mime/xdgmimeint.c third_party/xdg_mime/xdgmimemagic.c third_party/xdg_mime/xdgmimeparent.c third_party/xdg_user_dirs/xdg_user_dir_lookup.cc)

# time
ADD_LIBRARY(time OBJECT third_party/nspr/prtime.cc time/time.cc time/time_posix.cc time/clock.cc time/tick_clock.cc time/default_clock.cc time/default_tick_clock.cc)

# timer
ADD_LIBRARY(timer OBJECT timer/timer.cc timer/elapsed_timer.cc timer/hi_res_timer_manager_posix.cc)

# files
ADD_LIBRARY(files OBJECT files/file.cc files/file_posix.cc files/file_path.cc files/file_path_constants.cc files/file_path_watcher.cc files/file_path_watcher_linux.cc files/file_tracing.cc files/file_enumerator.cc files/file_enumerator_posix.cc files/file_descriptor_watcher_posix.cc files/file_proxy.cc files/file_util.cc files/file_util_linux.cc files/file_util_posix.cc files/file_util_proxy.cc files/important_file_writer.cc files/memory_mapped_file.cc files/memory_mapped_file_posix.cc files/scoped_file.cc files/scoped_temp_dir.cc path_service.cc)

# metrics
ADD_LIBRARY(metrics OBJECT metrics/histogram.cc metrics/histogram_base.cc metrics/histogram_delta_serialization.cc metrics/histogram_snapshot_manager.cc metrics/histogram_samples.cc metrics/sparse_histogram.cc metrics/metrics_hashes.cc metrics/sample_map.cc metrics/sample_vector.cc metrics/statistics_recorder.cc metrics/persistent_sample_map.cc metrics/persistent_histogram_allocator.cc metrics/persistent_memory_allocator.cc metrics/bucket_ranges.cc metrics/field_trial.cc metrics/field_trial_param_associator.cc metrics/user_metrics.cc)

# rand_util
ADD_LIBRARY(randutil OBJECT rand_util.cc rand_util_posix.cc)

# hash
ADD_LIBRARY(hash OBJECT third_party/superfasthash/superfasthash.c hash.cc)

# base64
ADD_LIBRARY(base64 OBJECT ${MODP_DIR}/modp_b64.cc base64.cc base64url.cc)

# md5
ADD_LIBRARY(md5 OBJECT md5.cc)

# sha1
ADD_LIBRARY(sha1 OBJECT sha1.cc)

# values
ADD_LIBRARY(values OBJECT values.cc)

# power_monitor
ADD_LIBRARY(powermonitor OBJECT power_monitor/power_monitor.cc power_monitor/power_monitor_device_source.cc power_monitor/power_monitor_device_source_posix.cc power_monitor/power_monitor_source.cc)

# process
ADD_LIBRARY(process OBJECT process/launch.cc process/launch_posix.cc process/kill.cc process/kill_posix.cc process/process_linux.cc process/process_posix.cc process/process_handle.cc process/process_handle_linux.cc process/process_handle_posix.cc process/process_iterator.cc process/process_iterator_linux.cc process/process_metrics.cc process/process_metrics_linux.cc process/process_metrics_posix.cc process/memory.cc process/memory_linux.cc process/internal_linux.cc)

# profiler
ADD_LIBRARY(profiler OBJECT location.cc tracked_objects.cc tracking_info.cc profiler/scoped_profile.cc profiler/scoped_tracker.cc profiler/tracked_time.cc)

# environment
ADD_LIBRARY(environment OBJECT environment.cc)

# misc
ADD_LIBRARY(misc OBJECT barrier_closure.cc big_endian.cc build_time.cc feature_list.cc guid.cc sequence_token.cc supports_user_data.cc sync_socket_posix.cc value_conversions.cc)


# all files refer to "base/...", so we include ".." here
# we refer to ICU_FORK_DIR, because it contains the "uformattable.h" header
# we refer to CED_DIR, because it contains the "compact_enc_det.h" header
# we include CMAKE_BINARY_DIR because generated .h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${libevent_INCLUDE_DIRS} ${icui18n_INCLUDE_DIRS} ${ICU_FORK_DIR}/source/i18n ${CED_DIR}/src ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${libevent_LIBRARIES} ${icui18n_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lrt -ldl)

ADD_LIBRARY(base SHARED $<TARGET_OBJECTS:switches> $<TARGET_OBJECTS:command_line> $<TARGET_OBJECTS:strings> $<TARGET_OBJECTS:logging> $<TARGET_OBJECTS:i18n> $<TARGET_OBJECTS:debug> $<TARGET_OBJECTS:lock> $<TARGET_OBJECTS:memory> $<TARGET_OBJECTS:messageloop> $<TARGET_OBJECTS:nativelibrary> $<TARGET_OBJECTS:version> $<TARGET_OBJECTS:vlog> $<TARGET_OBJECTS:pickle> $<TARGET_OBJECTS:paths> $<TARGET_OBJECTS:atexit> $<TARGET_OBJECTS:callback> $<TARGET_OBJECTS:lazyinstance> $<TARGET_OBJECTS:posix> $<TARGET_OBJECTS:sysinfo> $<TARGET_OBJECTS:systemmonitor> $<TARGET_OBJECTS:taskscheduler>  $<TARGET_OBJECTS:threading> $<TARGET_OBJECTS:xdg> $<TARGET_OBJECTS:time> $<TARGET_OBJECTS:timer> $<TARGET_OBJECTS:files> $<TARGET_OBJECTS:metrics> $<TARGET_OBJECTS:randutil> $<TARGET_OBJECTS:hash> $<TARGET_OBJECTS:base64> $<TARGET_OBJECTS:md5> $<TARGET_OBJECTS:sha1> $<TARGET_OBJECTS:values> $<TARGET_OBJECTS:powermonitor> $<TARGET_OBJECTS:process> $<TARGET_OBJECTS:profiler> $<TARGET_OBJECTS:environment> $<TARGET_OBJECTS:misc> third_party/dynamic_annotations/dynamic_annotations.c)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(base ${link_libraries})

INSTALL(TARGETS base
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
