#include "combobox.h"
/*typedef struct ui_combobox ui_combobox;
struct ui_combobox{
	struct ui_element;

	ui_combobox** active;
	const char* text;
	void (*on_activation)(ui_combobox* self);
};

ui_combobox* ui_combobox_create(size_t x, size_t y, const char* text,
								ui_combobox** active,
								void (*on_activation)(ui_combobox*);*/

#include "draw.h"
#include <stdlib.h>

static void ui_combobox_on_input(ui_element* _self, utf_char_t char_in);
static void ui_combobox_on_draw(ui_element* _self);

ui_combobox* ui_combobox_create(size_t x, size_t y, const char* text,
								ui_combobox** active,
								void (*on_activation)(ui_combobox*))
{
	ui_combobox* _new = malloc(sizeof(ui_combobox));
	_new->x = x; _new->y = y;
	_new->w = input_utf_string_length(text) + 1; _new->h = 1;
	_new->text = text;
	_new->active = active;

	_new->on_activation = on_activation;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_combobox_on_input;
	_new->on_draw = ui_combobox_on_draw;

	return _new;
}

static void ui_combobox_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_combobox* self = (ui_combobox*)_self;
	const char** binds = input_char_to_binds(char_in);
	if(check_bind(binds, "activate_ui_element")){
		*self->active = self;
		if(self->on_activation)
			self->on_activation(self);
	}
}
static void ui_combobox_on_draw(ui_element* _self)
{
	ui_combobox* self = (ui_combobox*)_self;

	term_char_format bg_style;
	if(_self == self->parent_wnd->active_elem)
		bg_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		bg_style = (term_char_format){0};

	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, *self->active == self ? "◉" : "◯");
	term_setformat_raw(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, bg_style);
	term_draw_string(self->x + self->parent_wnd->x + 1, self->y + self->parent_wnd->y, self->text, bg_style);
}
