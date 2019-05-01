//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/liburl.so /usr/lib/chromium/lib/libcrypto.so /usr/lib/chromium/lib/libsdch.so /usr/lib/chromium/lib/libnet.so

#include <iostream>				// for << >>
#include <string>				// for std::string

#include "url/gurl.h"				// for GURL
#include "net/base/completion_callback.h"	// for net::CompletionCallback
#include "net/base/request_priority.h"		// for net::LOW
#include "net/http/http_request_info.h"		// for net::HttpRequestInfo
#include "net/http/http_basic_state.h"		// for net::HttpBasicState
#include "net/socket/client_socket_handle.h"	// for net::ClientSocketHandle


 // we construct a HTTP request out of an URL/method

int main (int argc, char *argv[])
{
	 // create a HTTP request

	net::HttpRequestInfo request;
	request.url = GURL ("https://www.google.fr/?gws_rd=ssl#q=tarnyko");
	request.method = "GET";

	 // initialize it (false = no proxy ; LOW = low priority)

	net::HttpBasicState state (std::unique_ptr<net::ClientSocketHandle> (new net::ClientSocketHandle ()), false, true);
	state.Initialize (&request, net::LOW, net::NetLogWithSource(), net::CompletionCallback());

	 // display it raw

	std::cout << "Request : " << state.GenerateRequestLine () << std::endl;

	return 0;
}

