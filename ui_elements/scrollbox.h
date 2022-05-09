#ifndef UI_ELEMENT_SCROLLBOX
#define UI_ELEMENT_SCROLLBOX

#include "ui_element.h"

typedef struct ui_scrollbox ui_scrollbox;
struct ui_scrollbox{
	struct ui_element;

	size_t scroll_pos;
	const char* text;
};

ui_scrollbox* ui_scrollbox_create(size_t x, size_t y,
									size_t w, size_t h,
									const char* text);

#endif
