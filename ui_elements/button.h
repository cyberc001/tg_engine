#ifndef UI_ELEMENT_BUTTON
#define UI_ELEMENT_BUTTON

#include "ui_element.h"

#define UI_BUTTON_STYLE_NONE		0
#define UI_BUTTON_STYLE_BRACKETS	1

typedef struct ui_button ui_button;
struct ui_button{
	struct ui_element;

	int style;
	const char* text;
	void (*on_press)(ui_button* self);
};

ui_button* ui_button_create(size_t x, size_t y,
							const char* text, int style,
							void (*on_press)(ui_button*));

#endif
