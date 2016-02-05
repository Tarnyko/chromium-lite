//  (C)2014 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so
//   Create "test.txt" to make it run

#include <iostream>					// for cout, endl...

#include "base/logging.h"				// for LOG()
#include "base/command_line.h"				// for CommandLine
#include "base/strings/string_number_conversions.h"	// for StringToInt()
#include "base/callback.h"				// for Callback
#include "base/bind.h"					// for Bind
#include "base/message_loop/message_loop.h"		// for MessageLoop
#include "base/run_loop.h"				// for RunLoop
#include "base/at_exit.h"				// for AtExitManager
#include "base/time/time.h"				// for TimeDelta


 // we run this function as a callback

int multiply_by (int factor, int number) {
	LOG(INFO) << "Launched multiply_by...";
	return factor*number;
}

 /* we run this function delayed by <X> seconds via a MessageLoop
  * (functions called by PostTask()/PostTaskDelayed() need to have void type) */

void multiply_by_delayed (int factor, int number) {
	LOG(INFO) << "Launched multiply_by_delayed...";
	LOG(INFO) << "Delayed result : " << factor*number;
}

int main (int argc, char *argv[])
{
	LOG(INFO) << "Hello !";

	auto cmdline = base::CommandLine (argc, argv);

	if (cmdline.HasSwitch ("help")) { // without the "--"
		std::cout << " <number> <seconds> : multiply a number by 2 after X seconds" << std::endl;
		std::cout << " --help             : this help text" << std::endl;
		return 0;
	} else if (argc < 3) {
		LOG(INFO) << "No good arguments, exiting ; try \"--help\"...";
		return 0;
	}

	int number, seconds;
	base::StringToInt (cmdline.GetArgs()[0], &number);
	base::StringToInt (cmdline.GetArgs()[1], &seconds);


	 // an "AtExitManager" is required for any to-be-PostTask()ed function beore its Bind()ing

	base::AtExitManager exit_manager;
	base::Callback<void()> callback_delayed = base::Bind (&multiply_by_delayed, 2, number);
	auto duration = base::TimeDelta::FromSeconds (seconds);

	base::MessageLoop main_loop;
	main_loop.PostDelayedTask (FROM_HERE, callback_delayed, duration);


	int result;
	base::Callback<int(int)> callback = base::Bind (&multiply_by, 2);
	result = callback.Run (number);
	LOG(INFO) << "Immediate result : " << result;


	 // main loop, needed to proces PostTask()ed functions
	 // we will stay here forever until we post a "run_loop.QuitClosure()" task
	base::RunLoop run_loop;
	run_loop.Run ();
	

	return 0;
}
