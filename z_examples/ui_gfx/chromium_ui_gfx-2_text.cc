//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so

#include <iostream>				// for << >>
#include <fstream>				// for ofstream

#include "base/strings/utf_string_conversions.h"// for ASCIIToUTF16
#include "skia/include/core/SkBitmap.h"		// for SkBitmap
#include "ui/gfx/render_text.h"			// for gfx::RenderText
#include "ui/gfx/codec/png_codec.h"		// for gfx::PNGCodec


int main (int argc, char *argv[])
{
	 // create a 320x240 blue Skia bitmap
	SkBitmap bitmap;
	bitmap.allocN32Pixels (320, 240);
	bitmap.eraseColor (SK_ColorBLUE);

	 // choose a yellow bold font
	auto *render_text = gfx::RenderText::CreateInstance ();
	render_text->SetColor (SK_ColorYELLOW);
	render_text->SetStyle (gfx::BOLD, true);

	 // write "Hello World !"
	render_text->SetText (base::ASCIIToUTF16 ("Hello"));
	render_text->MoveCursor (gfx::CHARACTER_BREAK, gfx::CURSOR_RIGHT, false);
	render_text->SetText (base::ASCIIToUTF16 ("World !"));

	 // convert it to PNG
	std::vector<unsigned char> png_data;
	gfx::PNGCodec::EncodeBGRASkBitmap(bitmap, false, &png_data);

	 // put the PNG data in a single string
	std::string data;
	data.insert(data.end(), png_data.begin(), png_data.end());

	 // write the file to the disk
	std::ofstream file;
	file.open ("picture2.png");
	file << data;
	file.close ();

	return 0;
}
