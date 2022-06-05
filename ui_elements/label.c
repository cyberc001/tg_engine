#include "label.h"

#include "draw.h"
#include <stdlib.h>

static void ui_label_on_draw(ui_element* _self);

ui_label* ui_label_create(size_t x, size_t y,
							size_t w, size_t h,
							int wrap, const char* text)
{
	ui_label* _new = malloc(sizeof(ui_label));
	_new->x = x; _new->y = y;
	_new->w = w; _new->h = h;
	_new->wrap = wrap;
	_new->text = text;

	_new->prev = _new->next = NULL;
	_new->flags = 0;
	_new->on_input = NULL;
	_new->on_draw = ui_label_on_draw;
	return _new;
}

static void ui_label_on_draw(ui_element* _self)
{
	ui_label* self = (ui_label*)_self;
	term_char_format bg_style = {0};
	term_char_format textfmt = (term_char_format){0};

	if(self->wrap){
		const char* tx = self->text;
		int x = self->x + self->parent_wnd->x;
		int y = self->y + self->parent_wnd->y;
		while(*tx){
			utf_char_t ch;
			input_utf_char_from_string(&tx, ch, &textfmt);
			term_char_format tmp_fmt = textfmt;
			merge_formats(&tmp_fmt, &bg_style);

			term_setchar(x, y, ch);
			term_setformat_raw(x, y, tmp_fmt);
			if(++x >= self->x + self->parent_wnd->x + self->w){
				x = self->x + self->parent_wnd->x;
				++y;
			}
		}
	}
	else
		term_draw_string(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, self->text, bg_style);
}
