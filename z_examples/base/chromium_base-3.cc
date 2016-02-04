//  (C)2014 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so
//   Create "test.txt" to make it run

#include <iostream>			// for cout, endl...

#include "base/logging.h"		// for LOG()
#include "base/files/file_path.h"	// for FilePath
#include "base/files/file_util.h"	// for PathExists(), DeleteFile()...
#include "base/command_line.h"		// for CommandLine


int main (int argc, char *argv[])
{
	LOG(INFO) << "Hello !";


	auto cmdline = base::CommandLine (argc, argv);

	if (cmdline.GetSwitches().empty()) {
		LOG(INFO) << "No parameters, exiting ; try \"--help\"...";
		return 0;
	}
	if (cmdline.HasSwitch ("help")) { // without the "--"
		std::cout << " -f=<file> -d=<directory> : move a file to a subdirectory" << std::endl;
		std::cout << " --help                   : this help text" << std::endl;
		return 0;
	}
	if (!cmdline.HasSwitch ("f") && !cmdline.HasSwitch ("d")) {
		LOG(INFO) << "Unrecognized parameters, exiting ; try \"--help\"...";
		return 0;
	}


	auto dirpath = cmdline.GetSwitchValuePath ("d");
	auto filepath = cmdline.GetSwitchValuePath ("f");
	// GetSwitchValueASCII also exists

	if (base::PathExists (filepath)) {
		LOG(INFO) << "Creating directory '" << dirpath.value() << "'...";
		base::CreateDirectory (dirpath);
		LOG(INFO) << "Copying file '" << filepath.value() << "' to directory...";
		dirpath = dirpath.Append (filepath); // concatenate
		base::CopyFile (filepath, dirpath);
		LOG(INFO) << "Deleting '" << filepath.value() << "'...";
		base::DeleteFile (filepath, false); // recursive = false
	} else {
		LOG(INFO) << "Please create '" << filepath.value() << "' to run the test !";
	}
	filepath.clear();
	dirpath.clear();


	return 0;
}
