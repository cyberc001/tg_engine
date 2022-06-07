#ifndef UI_ELEMENT_LISTBOX
#define UI_ELEMENT_LISTBOX

#include "ui_element.h"

typedef struct ui_listbox ui_listbox;
struct ui_listbox{
	struct ui_element;

	const char** items;
	size_t item_cnt;

	size_t selection;
	size_t scroll_pos;

	void (*on_press)(ui_listbox* self);
};

ui_listbox* ui_listbox_create(size_t x, size_t y,
									size_t w, size_t h,
									void (*on_press)(ui_listbox*));

void ui_listbox_add_item(ui_listbox* self, const char* item);
void ui_listbox_clear_items(ui_listbox* self, int free_items);

#endif
