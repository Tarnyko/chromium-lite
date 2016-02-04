//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/url /usr/lib/chromium/lib/libcrcrypto.so /usr/lib/chromium/lib/libbase.so -lnss3

#include <stdio.h>		// for printf
#include <stdlib.h>		// for atoi
#include <string>		// for std::string

#include "base/stl_util.h"	// for string_as_array
#include "base/base64.h"	// for base::Base64Encode
#include "crypto/random.h"	// for crypto::RandBytes


int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf ("Usage : ./chromium_crypto-1 <length>\n");
		return 0;
	}

	int length = atoi (argv[1]);

	std::string challenge (length, '\0');
	std::string encoded_challenge;

	crypto::RandBytes (string_as_array (&challenge), challenge.length ());
	base::Base64Encode (challenge, &encoded_challenge);

	printf ("Random challenge of length %d : %s\n", length, encoded_challenge.c_str ());

	return 0;
}
