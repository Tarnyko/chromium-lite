//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_events.so

#include <iostream>			// for << >>

#include "ui/gfx/geometry/point.h"	// for gfx::Point
#include "ui/events/event.h"		// for ui::MouseEvent
#include "ui/events/event_utils.h"	// for ui::EventTimeForNow()


 // we click 3 times, and verify the event has been intercepted

int main (int argc, char *argv[])
{
	gfx::Point origin (0, 0);
	int result;

	ui::MouseEvent ev (ui::ET_MOUSE_PRESSED, origin, origin, ui::EventTimeForNow(), 0, 0);
	ev.SetClickCount (3);

	result = ev.GetClickCount ();
	std::cout << "Clicked " << result << " times." << std::endl;

	return 0;
}
