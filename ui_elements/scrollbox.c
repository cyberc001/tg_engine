#include "scrollbox.h"

#include "draw.h"
#include <stdlib.h>

static void ui_scrollbox_on_input(ui_element* _self, utf_char_t char_in);
static void ui_scrollbox_on_draw(ui_element* _self);

ui_scrollbox* ui_scrollbox_create(size_t x, size_t y,
									size_t w, size_t h,
									const char* text)
{
	ui_scrollbox* _new = malloc(sizeof(ui_scrollbox));
	_new->x = x; _new->y = y;
	_new->w = w; _new->h = h;
	_new->text = text;
	_new->scroll_pos = 0;

	_new->prev = _new->next = NULL;
	_new->flags = UI_ELEMENT_FLAG_CAN_BE_ACTIVE;
	_new->on_input = ui_scrollbox_on_input;
	_new->on_draw = ui_scrollbox_on_draw;
	return _new;
}

static void ui_scrollbox_on_input(ui_element* _self, utf_char_t char_in)
{
	ui_scrollbox* self = (ui_scrollbox*)_self;
	const char** binds = input_char_to_binds(char_in);
	if(check_bind(binds, "ui_element_up")){
		if(self->scroll_pos > 0)
			--self->scroll_pos;
	}
	else if(check_bind(binds, "ui_element_down")){
		size_t text_ln = input_utf_string_length(self->text);
		size_t text_lines = (text_ln + (self->w - 2)) / (self->w - 1);
		size_t max_scroll_pos = text_lines > self->h ? text_lines - self->h : 0;
		if(self->scroll_pos + 1 <= max_scroll_pos)
			++self->scroll_pos;
	}
}
static void ui_scrollbox_on_draw(ui_element* _self)
{
	ui_scrollbox* self = (ui_scrollbox*)_self;
	term_char_format bg_style;
	if(_self == _self->parent_wnd->active_elem)
		bg_style = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
	else
		bg_style = (term_char_format){0};
	term_char_format textfmt = (term_char_format){0};

	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y, "▲");
	term_draw_line(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + 1, self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + self->h - 2, "░", (term_char_format){0});
	term_setchar(self->x + self->parent_wnd->x, self->y + self->parent_wnd->y + self->h - 1, "▼");

	size_t text_ln = input_utf_string_length(self->text);
	size_t text_lines = (text_ln + (self->w - 2)) / (self->w - 1);
	size_t max_scroll_pos = text_lines > self->h ? text_lines - self->h : 0;
	int slider_area = self->h - 2;
	int slider_y = self->y + self->parent_wnd->y + 1 + (self->scroll_pos * slider_area) / max_scroll_pos;
	term_setchar(self->x + self->parent_wnd->x, slider_y, "█");

	term_draw_rect(self->x + self->parent_wnd->x + 1, self->y + self->parent_wnd->y, self->x + self->parent_wnd->x + self->w - 1, self->y + self->parent_wnd->y + self->h - 1, " ", bg_style);

	const char* tx = self->text;
	int x = self->x + self->parent_wnd->x + 1;
	int y = self->y + self->parent_wnd->y;
	size_t skip = self->scroll_pos * (self->w - 1);
	while(*tx){
		utf_char_t ch;
		input_utf_char_from_string(&tx, ch, &textfmt);
		term_char_format tmp_fmt = textfmt;
		merge_formats(&tmp_fmt, &bg_style);

		if(skip)
		{ --skip; continue; }
		term_setchar(x, y, ch);
		term_setformat_raw(x, y, tmp_fmt);
		if(++x >= self->x + self->parent_wnd->x + self->w){
			x = self->x + self->parent_wnd->x + 1;
			++y;
			if(y >= self->y + self->parent_wnd->y + self->h)
				break;
		}
	}
}
