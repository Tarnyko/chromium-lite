//  (C)2016 Tarnyko <tarnyko@tarnyko.net>
// compile with : g++ -std=c++11 -DUSE_X11/-DUSE_OZONE -DUSE_AURA ... -I/usr/include/chromium /usr/lib/chromium/lib/libbase.so /usr/lib/chromium/lib/libskia.so /usr/lib/chromium/lib/libipc.so /usr/lib/chromium/lib/libui_gfx.so /usr/lib/chromium/lib/libui_accessibility.so

#include <iostream>			    // for << >>
#include <vector>			    // for std::vector

#include "base/strings/utf_string_conversions.h" // for base::UTF8ToUTF16
#include "ui/accessibility/ax_text_utils.h" // for ui::FindAccessibleTextBoundary


 // we are testing text boundaries in a left-to-right manner

int main (int argc, char *argv[])
{
	base::string16 text;
	std::vector<int> line_offsets;
	size_t result;

	text = base::UTF8ToUTF16 ("Text 1.\nText2.\n");
	line_offsets.push_back (8);
	line_offsets.push_back (15);

	result = ui::FindAccessibleTextBoundary (text, line_offsets, ui::LINE_BOUNDARY, 5, ui::FORWARDS_DIRECTION);

	std::cout << "Result : " << result << std::endl;

	return 0;
}
