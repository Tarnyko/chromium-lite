//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libipc.so

#include <iostream>		// for << >>
#include <string>		// for std::string

#include "base/pickle.h"	// for base::PickleIterator
#include "ipc/ipc_message.h"	// for IPC::Message


 /* we create a message and verify it contains the correct value */

int main (int argc, char *argv[])
{
	std::string hello ("Hello !");
	std::string res_str;
	bool res;

	IPC::Message msg (0, 1, IPC::Message::PRIORITY_NORMAL);
	msg.WriteString (hello);

	base::PickleIterator iter (msg);
	res = iter.ReadString (&res_str);

	std::cout << "Result : " << res_str << std::endl;

	return 0;
}

