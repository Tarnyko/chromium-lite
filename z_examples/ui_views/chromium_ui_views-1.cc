//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libui_gl.so /usr/lib/chromium/lib/libgpu.so /usr/lib/chromium/lib/libcc.so /usr/lib/chromium/lib/libui_compositor.so

#include <iostream>			// for << >>

#include "ui/views/widget/native_widget.h"


int main (int argc, char *argv[])
{
	auto *widget = new views::Widget;

	return 0;
}
