//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so /usr/lib/chromium/lib/libui_gl.so /usr/lib/chromium/lib/libgpu.so /usr/lib/chromium/lib/libcc.so

#include <iostream>			// for << >>

#include "ui/compositor/compositor.h"	// for ui::Compositor


int main (int argc, char *argv[])
{
	scoped_ptr<ui::Compositor> compositor;

	compositor->SetAcceleratedWidget (gfx::kNullAcceleratedWidget);

	return 0;
}
