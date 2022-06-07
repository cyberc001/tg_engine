#include "button.h"

#include "draw.h"
#include <stdlib.h>

static void ui_button_on_input(ui_element* _self, utf_char_t char_in);
static void ui_button_on_draw(ui_element* _self);

ui_button* ui_button_create(size_t x, size_t y,
							const char* text, int style,
							void (*on_press)(ui_button*)){
	ui_button* _new = malloc(sizeof(ui_button));
	_new->x = x; _new->y = y;
	_new->w = input_utf_string_length(text); _new->h = 1;
	_new->style = style;
	switch(style){
		case UI_BUTTON_STYLE_BRACKETS:
			_new->w += 2; break;
	}
	_new->text = text;
	_new->on_press = on_press;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_button_on_input;
	_new->on_draw = ui_button_on_draw;
	_new->on_destroy = NULL;

	return _new;
}

static void ui_button_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_button* self = (ui_button*)_self;
	const char** binds = input_char_to_binds(char_in);
	if(check_bind(binds, "activate_ui_element") && self->on_press)
		self->on_press(self);
}
static void ui_button_on_draw(ui_element* _self)
{
	ui_button* self = (ui_button*)_self;
	term_char_format bg_style;
	if(_self == self->parent_wnd->active_elem)
		bg_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		bg_style = (term_char_format){0};

	switch(self->style){
		case UI_BUTTON_STYLE_NONE:
		{
			term_draw_string(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, self->text, bg_style);
		} break;
		case UI_BUTTON_STYLE_BRACKETS:
		{
			term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, "[");
			term_setformat_raw(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, bg_style);
			int x = term_draw_string(self->x + self->parent_wnd->x + 1, self->y + self->parent_wnd->y, self->text, bg_style);
			term_setchar(x, self->y + self->parent_wnd->y, "]");
			term_setformat_raw(x, self->y + self->parent_wnd->y, bg_style);
		} break;
	}
}
