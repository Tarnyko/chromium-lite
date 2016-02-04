//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 ... -I/usr/include/url /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libui_gfx.so

#include <stdio.h>			// for printf

//#include "skia/include/core/SkTypes.h"	// REQUIRED !
#include "ui/gfx/geometry/rect.h"	// for gfx::Rect


int main (int argc, char *argv[])
{
	
	gfx::Rect rect (320, 240); // width, height

	return 0;
}
