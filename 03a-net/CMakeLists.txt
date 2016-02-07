PROJECT(chromium-net C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Net")

# Variable paths for required third-parties
SET(PROTOC_WRAPPER_DIR "..")
SET(GRIT_DIR "..")
SET(SDCH_DIR "../sdch")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(FindPythonLibs)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
# (we use NSS because OpenSSL requires Google BoringSSL)
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
ADD_DEFINITIONS(-DNET_IMPLEMENTATION -DUSE_NSS_CERTS)
# Required for crssl
ADD_DEFINITIONS(-DNO_PKCS11_BYPASS -DNSS_ENABLE_ECC -DUSE_UTIL_DIRECTLY)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -fpermissive)
ENDIF()


# PRE-GENERATION .PROTO -> .CC,.H,.PY

MESSAGE(STATUS "Generating .cc,.h,.py files from .proto...")

EXECUTE_PROCESS(COMMAND mkdir -p net/quic/proto WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include base/net_export.h --protobuf ${CMAKE_BINARY_DIR}/net/quic/proto/cached_network_parameters.pb.h --proto-in-dir quic/proto --proto-in-file cached_network_parameters.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=NET_EXPORT_PRIVATE:${CMAKE_BINARY_DIR}/net/quic/proto --python_out ${CMAKE_BINARY_DIR}/net/quic/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ${PROTOC_WRAPPER_DIR}/tools/protoc_wrapper/protoc_wrapper.py --include base/net_export.h --protobuf ${CMAKE_BINARY_DIR}/net/quic/proto/source_address_token.pb.h --proto-in-dir quic/proto --proto-in-file source_address_token.proto --use-system-protobuf=1 -- protoc --cpp_out dllexport_decl=NET_EXPORT_PRIVATE:${CMAKE_BINARY_DIR}/net/quic/proto --python_out ${CMAKE_BINARY_DIR}/net/quic/proto WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

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
ADD_LIBRARY(http OBJECT http/disk_based_cert_cache.cc http/disk_cache_based_quic_server_info.cc http/failing_http_transaction_factory.cc http/http_auth.cc http/http_auth_cache.cc http/http_auth_controller.cc http/http_auth_filter.cc http/http_auth_handler.cc http/http_auth_handler_basic.cc http/http_auth_handler_digest.cc http/http_auth_handler_factory.cc http/http_auth_handler_negotiate.cc http/http_auth_handler_ntlm.cc http/http_auth_handler_ntlm_portable.cc http/http_auth_multi_round_parse.cc http/http_auth_preferences.cc http/http_auth_scheme.cc http/http_auth_challenge_tokenizer.cc http/http_basic_state.cc http/http_basic_stream.cc http/http_byte_range.cc http/http_cache.cc http/http_cache_transaction.cc http/http_chunked_decoder.cc http/http_content_disposition.cc http/http_log_util.cc http/http_network_layer.cc http/http_network_session.cc http/http_network_session_peer.cc http/http_network_transaction.cc http/http_proxy_client_socket.cc http/http_proxy_client_socket_pool.cc http/http_proxy_client_socket_wrapper.cc http/http_request_info.cc http/http_request_headers.cc http/http_response_body_drainer.cc http/http_response_headers.cc http/http_response_info.cc http/http_security_headers.cc http/http_server_properties.cc http/http_server_properties_impl.cc http/http_server_properties_manager.cc http/http_status_code.cc http/http_status_line_validator.cc http/http_stream_factory.cc http/http_stream_factory_impl.cc http/http_stream_factory_impl_job.cc http/http_stream_factory_impl_request.cc http/http_util.cc http/http_util_icu.cc http/http_vary_data.cc http/md4.cc http/partial_data.cc http/proxy_client_socket.cc http/proxy_connect_redirect_http_stream.cc http/transport_security_persister.cc  http/transport_security_state.cc http/url_security_manager.cc http/url_security_manager_posix.cc)
# we removed "http/des.cc" because it requires OpenSSL (and is insecure !)

# log
ADD_LIBRARY(log OBJECT log/net_log.cc log/net_log_capture_mode.cc log/net_log_util.cc log/trace_net_log_observer.cc log/write_to_file_net_log_observer.cc)

# crssl
# Some quic source files below such as "aead_base_encrypter/decrypter_nss.cc"
# require the "CK_NSS_AEAD_PARAMS" implementation, which is not merged in
# upstream NSS for now (https://bugzilla.mozilla.org/show_bug.cgi?id=917571).
# Thus, we need to build this.
ADD_LIBRARY(crssl OBJECT third_party/nss/ssl/authcert.c third_party/nss/ssl/bodge/secitem_array.c third_party/nss/ssl/cmpcert.c third_party/nss/ssl/derive.c third_party/nss/ssl/dtlscon.c third_party/nss/ssl/prelib.c third_party/nss/ssl/ssl3con.c third_party/nss/ssl/ssl3ecc.c third_party/nss/ssl/ssl3ext.c third_party/nss/ssl/ssl3gthr.c third_party/nss/ssl/sslauth.c third_party/nss/ssl/sslcon.c third_party/nss/ssl/ssldef.c third_party/nss/ssl/sslenum.c third_party/nss/ssl/sslerr.c third_party/nss/ssl/sslerrstrs.c third_party/nss/ssl/sslgathr.c third_party/nss/ssl/sslinfo.c third_party/nss/ssl/sslinit.c third_party/nss/ssl/sslmutex.c third_party/nss/ssl/sslnonce.c third_party/nss/ssl/sslplatf.c third_party/nss/ssl/sslreveal.c third_party/nss/ssl/sslsecur.c third_party/nss/ssl/sslsnce.c third_party/nss/ssl/sslsock.c third_party/nss/ssl/ssltrace.c third_party/nss/ssl/sslver.c third_party/nss/ssl/unix_err.c)

# quic
ADD_LIBRARY(quic OBJECT quic/congestion_control/cubic.cc quic/congestion_control/cubic_bytes.cc quic/congestion_control/general_loss_algorithm.cc quic/congestion_control/hybrid_slow_start.cc quic/congestion_control/loss_detection_interface.cc quic/congestion_control/pacing_sender.cc quic/congestion_control/prr_sender.cc quic/congestion_control/rtt_stats.cc quic/congestion_control/send_algorithm_interface.cc quic/congestion_control/tcp_cubic_bytes_sender.cc quic/congestion_control/tcp_cubic_sender.cc quic/congestion_control/tcp_loss_algorithm.cc quic/congestion_control/time_loss_algorithm.cc quic/crypto/aead_base_decrypter_nss.cc quic/crypto/aead_base_encrypter_nss.cc quic/crypto/aes_128_gcm_12_decrypter_nss.cc quic/crypto/aes_128_gcm_12_encrypter_nss.cc quic/crypto/cert_compressor.cc quic/crypto/channel_id.cc quic/crypto/channel_id_nss.cc quic/crypto/channel_id_chromium.cc quic/crypto/common_cert_set.cc quic/crypto/crypto_framer.cc quic/crypto/crypto_handshake.cc quic/crypto/crypto_handshake_message.cc quic/crypto/crypto_secret_boxer.cc quic/crypto/crypto_server_config_protobuf.cc quic/crypto/crypto_utils.cc quic/crypto/curve25519_key_exchange.cc quic/crypto/local_strike_register_client.cc quic/crypto/null_decrypter.cc quic/crypto/null_encrypter.cc quic/crypto/p256_key_exchange_nss.cc quic/crypto/proof_source_chromium_nss.cc quic/crypto/proof_verifier_chromium.cc quic/crypto/properties_based_quic_server_info.cc quic/crypto/quic_crypto_client_config.cc quic/crypto/quic_crypto_server_config.cc quic/crypto/quic_decrypter.cc quic/crypto/quic_encrypter.cc quic/crypto/quic_random.cc quic/crypto/quic_server_info.cc quic/crypto/scoped_evp_aead_ctx.cc quic/crypto/strike_register.cc quic/iovector.cc quic/network_connection.cc quic/p2p/quic_p2p_crypto_config.cc quic/p2p/quic_p2p_crypto_stream.cc quic/p2p/quic_p2p_session.cc quic/p2p/quic_p2p_stream.cc quic/port_suggester.cc quic/quic_address_mismatch.cc quic/quic_alarm.cc quic/quic_bandwidth.cc quic/quic_chromium_client_session.cc quic/quic_client_session_base.cc  quic/quic_clock.cc quic/quic_config.cc quic/quic_connection.cc quic/quic_connection_helper.cc quic/quic_connection_logger.cc quic/quic_connection_stats.cc quic/quic_crypto_client_stream.cc quic/quic_crypto_server_stream.cc quic/quic_crypto_stream.cc quic/quic_data_reader.cc quic/quic_data_writer.cc quic/quic_default_packet_writer.cc quic/quic_fec_group.cc quic/quic_fec_group_interface.cc quic/quic_flags.cc quic/quic_flow_controller.cc quic/quic_frame_list.cc quic/quic_framer.cc quic/quic_headers_stream.cc quic/quic_http_stream.cc quic/quic_http_utils.cc quic/quic_multipath_transmissions_map.cc quic/quic_packet_creator.cc quic/quic_packet_generator.cc quic/quic_packet_reader.cc quic/quic_protocol.cc quic/quic_received_packet_manager.cc quic/quic_reliable_client_stream.cc quic/quic_sent_entropy_manager.cc quic/quic_sent_packet_manager.cc quic/quic_server_id.cc quic/quic_session.cc quic/quic_socket_address_coder.cc quic/quic_spdy_session.cc quic/quic_spdy_stream.cc quic/quic_stream_factory.cc quic/spdy_utils.cc quic/quic_stream_sequencer.cc quic/quic_sustained_bandwidth_recorder.cc quic/quic_time.cc quic/quic_types.cc quic/quic_unacked_packet_map.cc quic/quic_utils.cc quic/quic_write_blocked_list.cc quic/reliable_quic_stream.cc quic/spdy_utils.cc quic/stream_sequencer_buffer.cc)

# proxy
ADD_LIBRARY(proxy OBJECT proxy/dhcp_proxy_script_fetcher.cc proxy/dhcp_proxy_script_fetcher_factory.cc proxy/multi_threaded_proxy_resolver.cc proxy/network_delegate_error_observer.cc proxy/polling_proxy_config_service.cc proxy/proxy_bypass_rules.cc proxy/proxy_config.cc proxy/proxy_config_service_fixed.cc proxy/proxy_config_service_linux.cc proxy/proxy_config_source.cc proxy/proxy_info.cc proxy/proxy_list.cc proxy/proxy_resolver_factory.cc proxy/proxy_resolver_script_data.cc proxy/proxy_script_decider.cc proxy/proxy_script_fetcher_impl.cc proxy/proxy_server.cc proxy/proxy_service.cc)

# socket
ADD_LIBRARY(socket OBJECT socket/client_socket_factory.cc socket/client_socket_handle.cc socket/client_socket_pool.cc socket/client_socket_pool_base.cc socket/client_socket_pool_manager.cc socket/client_socket_pool_manager_impl.cc socket/next_proto.cc socket/nss_ssl_util.cc socket/server_socket.cc socket/socket_descriptor.cc socket/socket_net_log_params.cc socket/socket_posix.cc socket/socks5_client_socket.cc socket/socks_client_socket.cc socket/socks_client_socket_pool.cc socket/ssl_client_socket.cc socket/ssl_client_socket_nss.cc socket/ssl_client_socket_pool.cc socket/ssl_server_socket_nss.cc socket/stream_socket.cc socket/tcp_client_socket.cc socket/tcp_server_socket.cc socket/tcp_socket_posix.cc socket/transport_client_socket_pool.cc socket/unix_domain_client_socket_posix.cc socket/unix_domain_server_socket_posix.cc socket/websocket_endpoint_lock_manager.cc socket/websocket_transport_client_socket_pool.cc socket/websocket_transport_connect_sub_job.cc)

# spdy
ADD_LIBRARY(spdy OBJECT spdy/buffered_spdy_framer.cc spdy/fuzzing/hpack_fuzz_util.cc spdy/hpack/hpack_constants.cc spdy/hpack/hpack_decoder.cc spdy/hpack/hpack_encoder.cc spdy/hpack/hpack_entry.cc spdy/hpack/hpack_header_table.cc spdy/hpack/hpack_huffman_table.cc spdy/hpack/hpack_input_stream.cc spdy/hpack/hpack_output_stream.cc spdy/hpack/hpack_static_table.cc spdy/spdy_alt_svc_wire_format.cc spdy/spdy_buffer.cc spdy/spdy_buffer_producer.cc spdy/spdy_frame_builder.cc spdy/spdy_frame_reader.cc spdy/spdy_framer.cc spdy/spdy_header_block.cc spdy/spdy_headers_block_parser.cc spdy/spdy_http_stream.cc spdy/spdy_http_utils.cc spdy/spdy_pinnable_buffer_piece.cc spdy/spdy_prefixed_buffer_reader.cc spdy/spdy_protocol.cc spdy/spdy_proxy_client_socket.cc spdy/spdy_read_queue.cc spdy/spdy_session.cc spdy/spdy_session_key.cc spdy/spdy_session_pool.cc spdy/spdy_stream.cc spdy/spdy_write_queue.cc)

# ssl
ADD_LIBRARY(ssl OBJECT ssl/channel_id_service.cc ssl/channel_id_store.cc ssl/client_cert_store_nss.cc ssl/client_key_store.cc ssl/default_channel_id_store.cc ssl/signed_certificate_timestamp_and_status.cc ssl/ssl_cert_request_info.cc ssl/ssl_cipher_suite_names.cc ssl/ssl_cipher_suite_names.cc ssl/ssl_client_auth_cache.cc ssl/ssl_config.cc ssl/ssl_config_service.cc ssl/ssl_config_service_defaults.cc ssl/ssl_info.cc ssl/ssl_key_logger.cc ssl/ssl_platform_key_task_runner.cc ssl/ssl_platform_key_nss.cc ssl/ssl_server_config.cc ssl/threaded_ssl_private_key.cc)

# udp
ADD_LIBRARY(udp OBJECT udp/udp_client_socket.cc udp/udp_net_log_parameters.cc udp/udp_server_socket.cc udp/udp_socket_posix.cc)

# url_request
ADD_LIBRARY(url_request OBJECT url_request/certificate_report_sender.cc url_request/data_protocol_handler.cc url_request/redirect_info.cc url_request/sdch_dictionary_fetcher.cc url_request/static_http_user_agent_settings.cc url_request/url_fetcher.cc url_request/url_fetcher_core.cc url_request/url_fetcher_delegate.cc url_request/url_fetcher_impl.cc url_request/url_fetcher_response_writer.cc url_request/url_range_request_job.cc url_request/url_request.cc url_request/url_request_backoff_manager.cc url_request/url_request_context.cc url_request/url_request_context_builder.cc url_request/url_request_context_getter.cc url_request/url_request_context_storage.cc url_request/url_request_data_job.cc url_request/url_request_error_job.cc url_request/url_request_filter.cc url_request/url_request_http_job.cc url_request/url_request_intercepting_job_factory.cc url_request/url_request_interceptor.cc url_request/url_request_job.cc url_request/url_request_job_factory.cc url_request/url_request_job_factory_impl.cc url_request/url_request_job_manager.cc url_request/url_request_netlog_params.cc url_request/url_request_redirect_job.cc url_request/url_request_simple_job.cc url_request/url_request_status.cc url_request/url_request_test_job.cc url_request/url_request_throttler_entry.cc url_request/url_request_throttler_manager.cc url_request/view_cache_helper.cc url_request/websocket_handshake_userdata_key.cc)

# websockets
ADD_LIBRARY(websockets OBJECT websockets/websocket_basic_handshake_stream.cc websockets/websocket_basic_stream.cc websockets/websocket_channel.cc websockets/websocket_deflate_parameters.cc websockets/websocket_deflate_predictor_impl.cc websockets/websocket_deflate_stream.cc websockets/websocket_deflater.cc websockets/websocket_errors.cc websockets/websocket_extension.cc websockets/websocket_extension_parser.cc websockets/websocket_frame.cc websockets/websocket_frame_parser.cc websockets/websocket_handshake_challenge.cc websockets/websocket_handshake_constants.cc websockets/websocket_handshake_request_info.cc websockets/websocket_handshake_response_info.cc websockets/websocket_handshake_stream_create_helper.cc websockets/websocket_inflater.cc websockets/websocket_stream.cc)

# proto_file ("file://" protocol)
ADD_LIBRARY(proto_file OBJECT base/directory_lister.cc url_request/file_protocol_handler.cc url_request/url_request_file_dir_job.cc url_request/url_request_file_job.cc)

# proto_ftp ("ftp://" protocol)
ADD_LIBRARY(proto_ftp OBJECT ftp/ftp_auth_cache.cc ftp/ftp_ctrl_response_buffer.cc ftp/ftp_directory_listing_parser.cc ftp/ftp_directory_listing_parser_ls.cc ftp/ftp_directory_listing_parser_vms.cc ftp/ftp_directory_listing_parser_windows.cc ftp/ftp_network_layer.cc ftp/ftp_network_session.cc ftp/ftp_network_transaction.cc ftp/ftp_response_info.cc ftp/ftp_server_type_histograms.cc ftp/ftp_util.cc url_request/ftp_protocol_handler.cc url_request/url_request_ftp_job.cc)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/net/quic/proto/cached_network_parameters.pb.cc ${CMAKE_BINARY_DIR}/net/quic/proto/source_address_token.pb.cc)


# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
# SDCH and, for NSS, additional "third_party/nss" headers are required
SET(include_dirs ${INCLUDE_DIRS} ${protobuf_INCLUDE_DIRS} ${nss_INCLUDE_DIRS} ${nspr_INCLUDE_DIRS} ${SDCH_DIR}/open-vcdiff/src ${CMAKE_SOURCE_DIR}/third_party/nss/ssl/bodge ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/.. ${CMAKE_BINARY_DIR})
SET(link_libraries ${protobuf_LIBRARIES} ${nss_LIBRARIES} ${nspr_LIBRARIES})

ADD_LIBRARY(net SHARED $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:cert> $<TARGET_OBJECTS:cookies> $<TARGET_OBJECTS:der> $<TARGET_OBJECTS:disk_cache> $<TARGET_OBJECTS:dns> $<TARGET_OBJECTS:filter> $<TARGET_OBJECTS:http> $<TARGET_OBJECTS:log> $<TARGET_OBJECTS:crssl> $<TARGET_OBJECTS:quic> $<TARGET_OBJECTS:proxy> $<TARGET_OBJECTS:socket> $<TARGET_OBJECTS:spdy> $<TARGET_OBJECTS:ssl> $<TARGET_OBJECTS:udp> $<TARGET_OBJECTS:url_request> $<TARGET_OBJECTS:websockets> $<TARGET_OBJECTS:proto_file> $<TARGET_OBJECTS:proto_ftp> $<TARGET_OBJECTS:generated> sdch/sdch_owner.cc third_party/mozilla_security_manager/nsKeygenHandler.cpp third_party/mozilla_security_manager/nsNSSCertificateDB.cpp third_party/mozilla_security_manager/nsPKCS12Blob.cpp)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(net ${link_libraries})

INSTALL(TARGETS net
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
