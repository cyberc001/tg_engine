#ifndef UI_ELEMENT
#define UI_ELEMENT

#include "window.h"

typedef struct ui_element ui_element;
struct ui_element{
	size_t x, y;
	size_t w, h;

	int can_be_active;

	void (*on_input)(ui_element* self, utf_char_t char_in);
	void (*on_draw)(ui_element* self);

	window* parent_wnd;
	struct ui_element *next, *prev;
};

#endif
