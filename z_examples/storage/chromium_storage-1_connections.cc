//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libsql.so /usr/lib/chromium/lib/libstorage.so

#include <iostream>				 // for << >>
#include <string>				 // for std::string

#include "base/strings/utf_string_conversions.h"
#include "storage/common/database/database_connections.h"


int main (int argc, char *argv[])
{
	std::string origin ("MyOrigin");
	base::string16 db (base::ASCIIToUTF16 ("MyDatabase"));
	bool result;

	auto *connections = new storage::DatabaseConnections;
	result = connections->IsDatabaseOpened (origin, db);

	if (result)
		std::cout << "'MyDatabase' is opened !" << std::endl;
	else
		std::cout << "'MyDatabase' is NOT opened !" << std::endl;

	return 0;
}
