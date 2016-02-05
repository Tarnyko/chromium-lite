//  (C)2014 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so

#include "base/logging.h"		// for LOG()


int main (int argc, char *argv[])
{
	LOG(INFO) << "Hello !";

	return 0;
}

