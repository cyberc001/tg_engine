#include "draw.h"
#include <stdlib.h>

#include <stdio.h>

int term_draw_string(int x, int y,
						const char* str, term_char_format fmt)
{
	term_char_format textfmt = (term_char_format){0};
	while(*str){
		utf_char_t ch;
		input_utf_char_from_string(&str, ch, &textfmt);
		term_char_format tmp_fmt = textfmt;
		merge_formats(&tmp_fmt, &fmt);

		term_setchar(x, y, ch);
		term_setformat_raw(x, y, tmp_fmt);
		if(++x > term_screen.bound_box.x1)
			return x;
	}
	return x;
}


void term_draw_line(int x0, int y0, int x1, int y1,
					const term_char_t ch, term_char_format fmt)
{
	if(x0 == x1){
		int sy = y0 < y1 ? 1 : -1;
		while(1){
			term_setchar(x0, y0, ch);
			term_setformat_raw(x0, y0, fmt);
			if(y0 == y1) return;
			y0 += sy;
		}
	}
	if(y0 == y1){
		int sx = x0 < x1 ? 1 : -1;
		while(1){
			term_setchar(x0, y0, ch);
			term_setformat_raw(x0, y0, fmt);
			if(x0 == x1) return;
			x0 += sx;
		}
	}

	int dx = abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;

	int err = dx + dy;
	while(1){
		term_setchar(x0, y0, ch);
		term_setformat_raw(x0, y0, fmt);
		if(x0 == x1 || y0 == y1)
			break;
		int err2 = 2 * err;
		if(err2 >= dy){
			if(x0 == x1)
				break;
			err += dy;
			x0 += sx;
		}
		if(err2 <= dx){
			if(y0 == y1)
				break;
			err += dx;
			y0 += sy;
		}
	}
}

void term_draw_rect(int x0, int y0, int x1, int y1,
					const term_char_t ch, term_char_format fmt)
{
	int rx = x0 > x1 ? x0 : x1;
	int ly = y0 > y1 ? y0 : y1;
	for(int uy = y0 < y1 ? y0 : y1; uy <= ly; ++uy)
		for(int lx = x0 < x1 ? x0 : x1; lx <= rx; ++lx){
			term_setchar(lx, uy, ch);
			term_setformat_raw(lx, uy, fmt);
		}
}
