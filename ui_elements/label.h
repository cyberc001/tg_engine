#ifndef UI_ELEMENT_LABEL
#define UI_ELEMENT_LABEL

#include "ui_element.h"

typedef struct ui_label ui_label;
struct ui_label{
	struct ui_element;

	int wrap;
	const char* text;
};

ui_label* ui_label_create(size_t x, size_t y,
							size_t w, size_t h,
							int wrap, const char* text);

#endif
