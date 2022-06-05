#include "checkbox.h"

#include "draw.h"
#include <stdlib.h>

static void ui_checkbox_on_input(ui_element* _self, utf_char_t char_in);
static void ui_checkbox_on_draw(ui_element* _self);

ui_checkbox* ui_checkbox_create(size_t x, size_t y, const char* text,
									void (*on_change)(ui_checkbox*))
{
	ui_checkbox* _new = malloc(sizeof(ui_checkbox));
	_new->x = x; _new->y = y;
	_new->w = input_utf_string_length(text) + 1; _new->h = 1;
	_new->text = text;
	_new->checked = 0;

	_new->on_change = on_change;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_checkbox_on_input;
	_new->on_draw = ui_checkbox_on_draw;

	return _new;
}

static void ui_checkbox_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_checkbox* self = (ui_checkbox*)_self;
	const char** binds = input_char_to_binds(char_in);
	if(check_bind(binds, "activate_ui_element")){
		self->checked = !self->checked;
		if(self->on_change)
			self->on_change(self);
	}
}
static void ui_checkbox_on_draw(ui_element* _self)
{
	ui_checkbox* self = (ui_checkbox*)_self;

	term_char_format bg_style;
	if(_self == self->parent_wnd->active_elem)
		bg_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		bg_style = (term_char_format){0};

	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, self->checked ? "☒" : "☐");
	term_setformat_raw(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, bg_style);
	term_draw_string(self->x + self->parent_wnd->x + 1, self->y + self->parent_wnd->y, self->text, bg_style);
}
