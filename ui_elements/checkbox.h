#ifndef UI_ELEMENT_CHECKBOX
#define UI_ELEMENT_CHECKBOX

#include "ui_element.h"

typedef struct ui_checkbox ui_checkbox;
struct ui_checkbox{
	struct ui_element;

	const char* text;
	int checked;
	void (*on_change)(ui_checkbox* self);
};

ui_checkbox* ui_checkbox_create(size_t x, size_t y, const char* text,
								void (*on_change)(ui_checkbox*));

#endif
