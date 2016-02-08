//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libui_base.so

#include <iostream>			// for << >>

#include "ui/base/layout.h"		// for ui::GetScaleForScaleFactor


 // we get the scale factor for some values

int main (int argc, char *argv[])
{
	std::cout << "Scale factor 100%% : " << ui::GetScaleForScaleFactor (ui::SCALE_FACTOR_100P) << std::endl;

	return 0;
}
