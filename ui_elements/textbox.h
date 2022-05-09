#ifndef UI_ELEMENT_TEXTBOX
#define UI_ELEMENT_TEXTBOX

#include "ui_element.h"

typedef struct ui_textbox ui_textbox;
struct ui_textbox{
	struct ui_element;

	char* input;
	size_t input_ln;
	void (*on_change)(ui_textbox* self);
};

ui_textbox* ui_textbox_create(size_t x, size_t y, size_t w,
								size_t input_ln,
								void (*on_change)(ui_textbox*));

#endif
