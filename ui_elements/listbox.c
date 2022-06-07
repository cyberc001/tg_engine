#include "listbox.h"

#include "draw.h"
#include <stdlib.h>

static void ui_listbox_on_input(ui_element* _self, utf_char_t char_in);
static void ui_listbox_on_draw(ui_element* _self);

ui_listbox* ui_listbox_create(size_t x, size_t y,
									size_t w, size_t h,
									void (*on_press)(ui_listbox*))
{
	ui_listbox* _new = malloc(sizeof(ui_listbox));
	_new->x = x; _new->y = y;
	_new->w = w; _new->h = h;

	_new->items = NULL;
	_new->item_cnt = 0;
	_new->selection = 0;
	_new->scroll_pos = 0;
	_new->on_press = on_press;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_listbox_on_input;
	_new->on_draw = ui_listbox_on_draw;
	return _new;
}

static void ui_listbox_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_listbox* self = (ui_listbox*)_self;
	const char** binds = input_char_to_binds(char_in);
	if(check_bind(binds, "ui_element_up")){
		if(self->selection > 0){
			--self->selection;
			if(self->selection < self->scroll_pos)
				--self->scroll_pos;
		}
	}
	else if(check_bind(binds, "ui_element_down")){
		if(self->selection + 1 < self->item_cnt)
			++self->selection;
		if(self->selection >= self->scroll_pos + self->h)
			++self->scroll_pos;
	}
	else if(check_bind(binds, "activate_ui_element") && self->on_press)
		self->on_press(self);
}
static void ui_listbox_on_draw(ui_element* _self)
{
	ui_listbox* self = (ui_listbox*)_self;
	term_char_format bg_style = {0};

	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, "▲");
	term_draw_line(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + 1, self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + self->h - 2, "░", (term_char_format){0});
	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + self->h - 1, "▼");
;
	int slider_area = self->h - 2;
	int slider_y = self->y + self->parent_wnd->y + 1 + (self->item_cnt == 0 ? 0 : (self->selection * slider_area) / self->item_cnt);
	if(self->selection == self->item_cnt - 1)
		slider_y = self->y + self->parent_wnd->y + self->h - 2;

	term_setchar(self->x + self->parent_wnd->x, slider_y, "█");

	term_draw_rect(self->x + self->parent_wnd->x + 1, self->y + self->parent_wnd->y, self->x + self->parent_wnd->x + self->w - 1, self->y + self->parent_wnd->y + self->h - 1, " ", bg_style);

	term_char_format selection_text_style;
	if(_self == _self->parent_wnd->active_elem)
		selection_text_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		selection_text_style = (term_char_format){0};
	int y = self->y + self->parent_wnd->y;
	for(size_t i = self->scroll_pos; i < self->scroll_pos + self->h && i < self->item_cnt; ++i, ++y){
		term_draw_string(self->x + self->parent_wnd->x + 1, y, self->items[i], i == self->selection ? selection_text_style : (term_char_format){0});
	}
}


void ui_listbox_add_item(ui_listbox* self, const char* item)
{
	++self->item_cnt;
	self->items = realloc(self->items, sizeof(char*) * self->item_cnt);
	self->items[self->item_cnt - 1] = item;
}

void ui_listbox_clear_items(ui_listbox* self, int free_items)
{
	if(free_items)
		for(size_t i = 0; i < self->item_cnt; ++i)
			free((char*)self->items[i]);
	free(self->items);
	self->items = NULL;
	self->item_cnt = 0;
}
