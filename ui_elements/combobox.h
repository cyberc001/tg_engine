#ifndef UI_ELEMENT_COMBOBOX
#define UI_ELEMENT_COMBOBOX

#include "ui_element.h"

typedef struct ui_combobox ui_combobox;
struct ui_combobox{
	struct ui_element;

	ui_combobox** active;
	const char* text;
	void (*on_activation)(ui_combobox* self);
};

ui_combobox* ui_combobox_create(size_t x, size_t y, const char* text,
								ui_combobox** active,
								void (*on_activation)(ui_combobox*));

#endif
