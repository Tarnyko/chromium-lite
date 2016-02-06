//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libnet.so /usr/lib/chromium/lib/liburl.so /usr/lib/chromium/lib/libbase.so

#include <iostream>		// for << >>
#include <string>		// for std::string

#include "net/base/ip_address.h"


 /* we XXX */

int main (int argc, char *argv[])
{
	uint8_t addr[4] = { 192, 168, 0, 1 };

	auto ip_address = net::IPAddress (addr, 4);

	return 0;
}
