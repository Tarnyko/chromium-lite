//  (C)2014 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so
//   Create "test.txt" to make it run

#include <iostream>					// for cout, endl...

#include "base/logging.h"				// for LOG()
#include "base/command_line.h"				// for CommandLine
#include "base/strings/string_number_conversions.h"	// for StringToInt()
#include "base/callback.h"				// for Callback
#include "base/bind.h"					// for Bind


void multiply_by_itself (int number) {
	LOG(INFO) << "Result : " << number*number;
}

int main (int argc, char *argv[])
{
	LOG(INFO) << "Hello !";

	auto cmdline = base::CommandLine (argc, argv);

	if (cmdline.HasSwitch ("help")) { // without the "--"
		std::cout << " <number> : multiply a number by itself" << std::endl;
		std::cout << " --help   : this help text" << std::endl;
		return 0;
	} else if (cmdline.GetArgs().empty()) {
		LOG(INFO) << "No arguments, exiting ; try \"--help\"...";
		return 0;
	}

	int number;
	base::StringToInt (cmdline.GetArgs()[0], &number);

	base::Callback<void(int)> callback = base::Bind (&multiply_by_itself);
	callback.Run (number);

	return 0;
}
