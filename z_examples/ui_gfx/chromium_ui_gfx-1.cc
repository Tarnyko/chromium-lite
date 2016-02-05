//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libui_gfx.so

#include <iostream>			// for << >>
#include <fstream>			// for ofstream

#include "skia/include/core/SkBitmap.h"	// for SkBitmap
#include "ui/gfx/codec/png_codec.h"	// for gfx::PNGCodec


int main (int argc, char *argv[])
{
	 // create a 320x240 red Skia bitmap
	SkBitmap bitmap;
	bitmap.allocN32Pixels (320, 240);
	bitmap.eraseColor (SK_ColorRED);

	 // convert it to PNG
	std::vector<unsigned char> png_data;
	gfx::PNGCodec::EncodeBGRASkBitmap(bitmap, false, &png_data);

	 // put the PNG data in a single string
	std::string data;
	data.insert(data.end(), png_data.begin(), png_data.end());

	 // write the file to the disk
	std::ofstream file;
	file.open ("picture.png");
	file << data;
	file.close ();

	return 0;
}
