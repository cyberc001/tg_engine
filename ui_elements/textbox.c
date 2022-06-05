#include "textbox.h"

#include "draw.h"
#include <stdlib.h>

static void ui_textbox_on_input(ui_element* _self, utf_char_t char_in);
static void ui_textbox_on_draw(ui_element* _self);

ui_textbox* ui_textbox_create(size_t x, size_t y, size_t w,
								size_t input_ln,
								void (*on_change)(ui_textbox*))
{
	ui_textbox* _new = malloc(sizeof(ui_textbox));
	_new->x = x; _new->y = y;
	_new->w = w; _new->h = 1;

	_new->input = malloc(input_ln + 1);
	_new->input[0] = 0;
	_new->input_ln = input_ln;
	_new->on_change = on_change;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_textbox_on_input;
	_new->on_draw = ui_textbox_on_draw;

	return _new;
}

static void ui_textbox_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_textbox* self = (ui_textbox*)_self;
	char* inp; for(inp = self->input; *inp; ++inp) {}

	if(char_in[0] == '\x7f'){
		if(inp > self->input){
			--inp;
			if(*inp & 128){
				while(inp > self->input && *inp & 64)
					--inp;
				--inp;
			}
			*inp = '\0';
		}
	}
	else if(char_in[0] != '\n'){
		if(inp - self->input > self->input_ln - strlen(char_in))
			return;
		for(char* i = char_in; *i; ++i, ++inp)
			*inp = *i;
		*inp = '\0';
	}
}
static void ui_textbox_on_draw(ui_element* _self)
{
	ui_textbox* self = (ui_textbox*)_self;
	term_char_format bg_style;
	if(_self == self->parent_wnd->active_elem)
		bg_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		bg_style = (term_char_format){0};

	term_draw_line(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, self->x + self->parent_wnd->x + self->w, self->y + self->parent_wnd->y, "_", bg_style);
	bg_style.flags |= CHAR_FORMAT_UNDERLINE;

	if(_self != self->parent_wnd->active_elem){
		char* inp = self->input;
		for(size_t i = 0; *inp && i <= self->w; ++i){
			utf_char_t ch;
			input_utf_char_from_string((const char**)&inp, ch, NULL);
		}
		char rem = *inp;
		*inp = '\0';
		term_draw_string(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, self->input, bg_style);
		*inp = rem;
	}
	else{
		size_t inc = input_utf_string_length(self->input) > self->w + 1 ? input_utf_string_length(self->input) - self->w - 1 : 0;
		const char* inp = self->input;
		for(size_t i = 0; *inp && i < inc; ++i){
			utf_char_t ch;
			input_utf_char_from_string(&inp, ch, NULL);
		}
		term_draw_string(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, inp, bg_style);
	}
}
