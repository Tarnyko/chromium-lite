//  (C)2014 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so
//   Create "test.txt" to make it run

#include "base/logging.h"		// for LOG()
#include "base/files/file_path.h"	// for FilePath
#include "base/files/file_util.h"	// for PathExists(), DeleteFile()...


int main (int argc, char *argv[])
{
	LOG(INFO) << "Hello !";

	 // if "test.txt" exists, we create a "test" folder and move it there

	auto dirpath = base::FilePath ("test");
	auto filepath = base::FilePath ("test.txt");

	if (base::PathExists (filepath)) {

		LOG(INFO) << "Creating directory '" << dirpath.value() << "'...";
		base::CreateDirectory (dirpath);

		LOG(INFO) << "Copying file '" << filepath.value() << "' to directory...";
		dirpath = dirpath.Append (filepath); // concatenate
		base::CopyFile (filepath, dirpath);

		LOG(INFO) << "Deleting '" << filepath.value() << "'...";
		base::DeleteFile (filepath, false); // recursive = false

	} else {
		LOG(INFO) << "Please create 'test.txt' to run the test !";
	}

	filepath.clear();
	dirpath.clear();

	return 0;
}
