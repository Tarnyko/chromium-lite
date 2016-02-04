//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/url /usr/lib/chromium/lib/liburl_lib.so /usr/lib/chromium/lib/libbase.so

#include "url/gurl.h"	// for GURL


int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("Usage : ./chromium_url-1 <url>\n");
		return 0;
	}

	GURL url (argv[1]);

	if (!url.is_valid ()) {
		printf ("URL %s is invalid... Exiting\n", argv[1]);
		return -1;
	} else {
		printf ("URL %s is valid !\n", argv[1]);
	}

	GURL url_origin = url.GetOrigin ();
	std::string content = url_origin.GetContent ();

	printf ("URL origin content is %s\n", content.c_str ());

	return 0;
}
