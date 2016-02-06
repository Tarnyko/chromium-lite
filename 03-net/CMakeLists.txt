PROJECT(chromium-net C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Net")

# Variable paths for required third-party tools
SET(PROTOC_WRAPPER_DIR "..")
SET(GRIT_DIR "..")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(FindPythonLibs)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(protobuf REQUIRED protobuf)
PKG_CHECK_MODULES(nss REQUIRED nss)
PKG_CHECK_MODULES(nspr REQUIRED nspr)

IF(NOT PYTHONLIBS_FOUND)
  MESSAGE(FATAL_ERROR "Python is required to generate some source files !")
ENDIF(NOT PYTHONLIBS_FOUND)

FIND_PROGRAM(PROTOC_FOUND protoc)
IF(NOT PROTOC_FOUND)
  MESSAGE(FATAL_ERROR "\"protoc\" is required, please install the \"protobuf-compiler\" package !")
ELSE()
  MESSAGE(STATUS "\"protoc\" found...")
ENDIF(NOT PROTOC_FOUND)

# Common definitions
ADD_DEFINITIONS(-DNET_IMPLEMENTATION)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -fpermissive)
ENDIF()


# PRE-GENERATION .PROTO -> .CC,.H,.PY

MESSAGE(STATUS "Generating .cc,.h,.py files from .proto...")

EXECUTE_PROCESS(COMMAND mkdir -p protoc_out/net/quic/proto WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p pyproto/net/quic/proto WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include base/net_export.h --protobuf ${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto/cached_network_parameters.pb.h --proto-in-dir quic/proto --proto-in-file cached_network_parameters.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=NET_EXPORT_PRIVATE:${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto --python_out ${CMAKE_BINARY_DIR}/pyproto/net/quic/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include base/net_export.h --protobuf ${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto/source_address_token.pb.h --proto-in-dir quic/proto --proto-in-file source_address_token.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=NET_EXPORT_PRIVATE:${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto --python_out ${CMAKE_BINARY_DIR}/pyproto/net/quic/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating .cc from .gperf...")

EXECUTE_PROCESS(COMMAND mkdir -p net/base/registry_controlled_domains WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND tools/dafsa/make_dafsa.py base/registry_controlled_domains/effective_tld_names.gperf ${CMAKE_BINARY_DIR}/net/base/registry_controlled_domains/effective_tld_names-inc.cc WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating .pak from .grd...")

EXECUTE_PROCESS(COMMAND ${GRIT_DIR}/tools/grit/grit.py -i base/net_resources.grd build -f ${GRIT_DIR}/tools/gritsettings/resource_ids -o ${CMAKE_BINARY_DIR} --write-only-new=0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST, PER SECTION

# base
ADD_LIBRARY(base OBJECT base/address_family.cc base/address_list.cc base/address_tracker_linux.cc base/auth.cc base/backoff_entry.cc base/backoff_entry_serializer.cc base/chunked_upload_data_stream.cc base/connection_type_histograms.cc base/crypto_module_nss.cc base/data_url.cc base/elements_upload_data_stream.cc base/escape.cc base/file_stream.cc base/file_stream_context.cc base/file_stream_context_posix.cc base/filename_util.cc base/filename_util_internal.cc base/hash_value.cc base/host_mapping_rules.cc base/host_port_pair.cc base/int128.cc base/io_buffer.cc base/ip_address_number.cc base/ip_address.cc base/ip_endpoint.cc base/ip_pattern.cc base/keygen_handler.cc base/keygen_handler_nss.cc base/layered_network_delegate.cc base/load_timing_info.cc base/lookup_string_in_fixed_set.cc base/mime_sniffer.cc base/mime_util.cc base/net_errors.cc base/net_errors_posix.cc base/net_module.cc base/net_util.cc base/network_activity_monitor.cc base/network_change_notifier.cc base/network_change_notifier_linux.cc base/network_delegate.cc base/network_delegate_impl.cc base/network_interfaces.cc base/network_interfaces_linux.cc base/network_interfaces_posix.cc base/network_quality_estimator.cc base/nss_memio.c base/platform_mime_util_linux.cc base/port_util.cc base/prioritized_dispatcher.cc base/registry_controlled_domains/registry_controlled_domain.cc base/request_priority.cc base/sdch_dictionary.cc base/sdch_manager.cc base/sdch_net_log_params.cc base/sdch_observer.cc base/socket_performance_watcher.cc base/static_cookie_policy.cc base/test_data_stream.cc base/upload_bytes_element_reader.cc base/upload_data_stream.cc base/upload_element_reader.cc base/upload_file_element_reader.cc base/url_util.cc base/zap.cc base/filename_util_icu.cc base/net_string_util_icu.cc base/net_util_icu.cc)

# cert
ADD_LIBRARY(cert OBJECT cert/asn1_util.cc cert/cert_database.cc cert/cert_database_nss.cc cert/cert_policy_enforcer.cc cert/cert_status_flags.cc cert/cert_verifier.cc cert/cert_verify_proc.cc cert/cert_verify_proc_nss.cc cert/cert_verify_proc_whitelist.cc cert/cert_verify_result.cc cert/crl_set.cc cert/crl_set_storage.cc cert/ct_known_logs.cc cert/ct_log_response_parser.cc cert/ct_log_verifier.cc cert/ct_log_verifier_nss.cc cert/ct_objects_extractor_nss.cc cert/ct_serialization.cc cert/ct_signed_certificate_timestamp_log_param.cc cert/ct_verify_result.cc cert/ev_root_ca_metadata.cc cert/internal/certificate_policies.cc cert/internal/extended_key_usage.cc cert/internal/name_constraints.cc cert/internal/parse_certificate.cc cert/internal/signature_algorithm.cc cert/internal/signature_policy.cc cert/internal/verify_name_match.cc cert/internal/verify_signed_data.cc cert/jwk_serializer_nss.cc cert/multi_log_ct_verifier.cc cert/multi_threaded_cert_verifier.cc cert/nss_cert_database.cc cert/pem_tokenizer.cc cert/signed_certificate_timestamp.cc cert/test_root_certs.cc cert/test_root_certs_nss.cc cert/x509_cert_types.cc cert/x509_certificate.cc cert/x509_certificate_nss.cc cert/x509_certificate_net_log_param.cc cert/x509_util.cc cert/x509_util_nss.cc cert_net/cert_net_fetcher_impl.cc cert_net/nss_ocsp.cc)

# cookies
ADD_LIBRARY(cookies OBJECT cookies/canonical_cookie.cc cookies/cookie_constants.cc cookies/cookie_monster.cc cookies/cookie_options.cc cookies/cookie_store.cc cookies/cookie_util.cc cookies/parsed_cookie.cc)

# der
ADD_LIBRARY(der OBJECT der/input.cc der/parse_values.cc der/parser.cc der/tag.cc)

# disk_cache
ADD_LIBRARY(disk_cache OBJECT disk_cache/blockfile/addr.cc disk_cache/blockfile/backend_impl.cc disk_cache/blockfile/backend_impl_v3.cc disk_cache/blockfile/backend_worker_v3.cc disk_cache/blockfile/bitmap.cc disk_cache/blockfile/block_bitmaps_v3.cc disk_cache/blockfile/block_files.cc disk_cache/blockfile/disk_format.cc disk_cache/blockfile/entry_impl.cc disk_cache/blockfile/entry_impl_v3.cc disk_cache/blockfile/eviction.cc disk_cache/blockfile/eviction_v3.cc disk_cache/blockfile/file.cc disk_cache/blockfile/file_lock.cc disk_cache/blockfile/file_posix.cc disk_cache/blockfile/in_flight_backend_io.cc disk_cache/blockfile/in_flight_io.cc disk_cache/blockfile/index_table_v3.cc disk_cache/blockfile/mapped_file.cc disk_cache/blockfile/mapped_file_posix.cc disk_cache/blockfile/rankings.cc disk_cache/blockfile/sparse_control.cc disk_cache/blockfile/stats.cc disk_cache/blockfile/trace.cc disk_cache/blockfile/webfonts_histogram.cc disk_cache/cache_creator.cc disk_cache/cache_util.cc disk_cache/cache_util_posix.cc disk_cache/memory/mem_backend_impl.cc disk_cache/memory/mem_entry_impl.cc disk_cache/memory/mem_rankings.cc disk_cache/net_log_parameters.cc disk_cache/simple/simple_backend_impl.cc disk_cache/simple/simple_entry_format.cc disk_cache/simple/simple_entry_impl.cc disk_cache/simple/simple_entry_operation.cc disk_cache/simple/simple_index.cc disk_cache/simple/simple_index_file.cc disk_cache/simple/simple_index_file_posix.cc disk_cache/simple/simple_net_log_parameters.cc disk_cache/simple/simple_synchronous_entry.cc disk_cache/simple/simple_util.cc disk_cache/simple/simple_util_posix.cc disk_cache/simple/simple_version_upgrade.cc)

# dns
ADD_LIBRARY(dns OBJECT dns/address_sorter_posix.cc dns/dns_client.cc dns/dns_config_service.cc dns/dns_config_service_posix.cc dns/dns_hosts.cc  dns/dns_query.cc dns/dns_reloader.cc dns/dns_response.cc dns/dns_session.cc dns/dns_socket_pool.cc dns/dns_transaction.cc dns/dns_util.cc dns/host_cache.cc dns/host_resolver.cc dns/host_resolver_impl.cc dns/host_resolver_proc.cc dns/mapped_host_resolver.cc dns/mdns_cache.cc dns/mdns_client.cc dns/mdns_client_impl.cc dns/record_parsed.cc dns/record_rdata.cc dns/serial_worker.cc dns/single_request_host_resolver.cc)

# filter
ADD_LIBRARY(filter OBJECT filter/filter.cc filter/gzip_filter.cc filter/gzip_header.cc filter/sdch_filter.cc)

# http
ADD_LIBRARY(http OBJECT http/des.cc http/disk_based_cert_cache.cc http/disk_cache_based_quic_server_info.cc http/failing_http_transaction_factory.cc http/http_auth.cc http/http_auth_cache.cc http/http_auth_controller.cc http/http_auth_filter.cc http/http_auth_handler.cc http/http_auth_handler_basic.cc http/http_auth_handler_digest.cc http/http_auth_handler_factory.cc http/http_auth_handler_negotiate.cc http/http_auth_handler_ntlm.cc http/http_auth_handler_ntlm_portable.cc http/http_auth_multi_round_parse.cc http/http_auth_preferences.cc http/http_auth_scheme.cc http/http_auth_challenge_tokenizer.cc http/http_basic_state.cc http/http_basic_stream.cc http/http_byte_range.cc http/http_cache.cc http/http_cache_transaction.cc http/http_chunked_decoder.cc http/http_content_disposition.cc http/http_log_util.cc http/http_network_layer.cc http/http_network_session.cc http/http_network_session_peer.cc http/http_network_transaction.cc http/http_proxy_client_socket.cc http/http_proxy_client_socket_pool.cc http/http_proxy_client_socket_wrapper.cc http/http_request_info.cc http/http_request_headers.cc http/http_response_body_drainer.cc http/http_response_headers.cc http/http_response_info.cc http/http_security_headers.cc http/http_server_properties.cc http/http_server_properties_impl.cc http/http_server_properties_manager.cc http/http_status_code.cc http/http_status_line_validator.cc http/http_stream_factory.cc http/http_stream_factory_impl.cc http/http_stream_factory_impl_job.cc http/http_stream_factory_impl_request.cc http/http_util.cc http/http_util_icu.cc http/http_vary_data.cc http/md4.cc http/partial_data.cc http/proxy_client_socket.cc http/proxy_connect_redirect_http_stream.cc http/transport_security_persister.cc  http/transport_security_state.cc http/url_security_manager.cc http/url_security_manager_posix)

# log
ADD_LIBRARY(log OBJECT log/net_log.cc log/net_log_capture_mode.cc log/net_log_util.cc log/trace_net_log_observer.cc log/write_to_file_net_log_observer.cc)

# socket
ADD_LIBRARY(socket OBJECT socket/client_socket_handle.cc socket/next_proto.cc socket/ssl_client_socket.cc)

# ssl
ADD_LIBRARY(ssl OBJECT ssl/channel_id_service.cc ssl/channel_id_store.cc ssl/client_key_store.cc ssl/default_channel_id_store.cc ssl/signed_certificate_timestamp_and_status.cc ssl/ssl_cert_request_info.cc ssl/ssl_cipher_suite_names.cc ssl/ssl_cipher_suite_names.cc ssl/ssl_client_auth_cache.cc ssl/ssl_config.cc ssl/ssl_config_service.cc ssl/ssl_info.cc ssl/ssl_server_config.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto/cached_network_parameters.pb.cc ${CMAKE_BINARY_DIR}/protoc_out/net/quic/proto/source_address_token.pb.cc)


# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${protobuf_INCLUDE_DIRS} ${nss_INCLUDE_DIRS} ${nspr_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR})
SET(link_libraries ${protobuf_LIBRARIES} ${nss_LIBRARIES} ${nspr_LIBRARIES})

ADD_LIBRARY(net SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:cert> $<TARGET_OBJECTS:cookies> $<TARGET_OBJECTS:der> $<TARGET_OBJECTS:disk_cache> $<TARGET_OBJECTS:dns> $<TARGET_OBJECTS:filter> $<TARGET_OBJECTS:http> $<TARGET_OBJECTS:log> $<TARGET_OBJECTS:socket> $<TARGET_OBJECTS:ssl> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(net ${link_libraries})

INSTALL(TARGETS net
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
