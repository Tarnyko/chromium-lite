//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libcrcrypto.so /usr/lib/chromium/lib/libbase.so

#include <iostream>		// for << >>
#include <string>		// for std::string
#include <stdlib.h>		// for atoi

#include "base/stl_util.h"	// for string_as_array
#include "base/base64.h"	// for base::Base64Encode
#include "crypto/random.h"	// for crypto::RandBytes


 /* we create a random string of specified <length>,
  * and then encode it with the Base64 scheme */

int main (int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage : ./chromium_crypto-1 <length>" << std::endl;
		return 0;
	}

	int length = atoi (argv[1]);

	std::string challenge (length, '\0');
	std::string encoded_challenge;

	crypto::RandBytes (base::string_as_array (&challenge), challenge.length ());
	base::Base64Encode (challenge, &encoded_challenge);

	std::cout << "Random challenge of length " << length << " : " <<  encoded_challenge << std::endl;

	return 0;
}
