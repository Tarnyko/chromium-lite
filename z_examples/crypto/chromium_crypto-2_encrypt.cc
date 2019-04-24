//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libcrcrypto.so /usr/lib/chromium/lib/libbase.so

#include <iostream>			// for << >>
#include <string>			// for std::string

#include "crypto/symmetric_key.h"	// for crypto::SymmetricKey
#include "crypto/encryptor.h"		// for crypto::Encryptor


 /* we create a random 256-bytes AES key,
  * and then encrypt/decrypt a string passed as command-line argument */

int main (int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage : ./chromium_crypto-2 <text>" << std::endl;
		return 0;
	}

	std::string plaintext (argv[1]);


	std::unique_ptr<crypto::SymmetricKey> key = crypto::SymmetricKey::GenerateRandomKey (crypto::SymmetricKey::AES, 256);

	 // display the AES key on screen

	std::string raw_key;
	key->GetRawKey (&raw_key);
	std::cout << "Random AES key : " << raw_key << std::endl;

	 // initialize the Encryptor with CBC scheme and a fixed 16-bytes salt

	crypto::Encryptor encryptor;
	std::string salt ("XXXXXXXXXXXXXXXX");
	std::string encrypted_str;
	std::string decrypted_str;

	encryptor.Init (key.release(), crypto::Encryptor::CBC, salt);
	encryptor.Encrypt (plaintext, &encrypted_str);
	encryptor.Decrypt (encrypted_str, &decrypted_str);

	std::cout << std::endl;
	std::cout << "Encrypted text : " << encrypted_str << std::endl;
	std::cout << "Decrypted text : " << decrypted_str << std::endl;

	return 0;
}
