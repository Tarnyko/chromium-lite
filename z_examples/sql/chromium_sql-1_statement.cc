//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libsql.so

#include <iostream>

#include "sql/connection.h"	// for sql::Connection
#include "sql/statement.h"	// for sql::Statement


int main (int argc, char *argv[])
{
	sql::Connection *db;
	sql::Statement s (db->GetUniqueStatement ("SELECT COUNT(*) FROM sqlite_master"));

	return 0;
}
