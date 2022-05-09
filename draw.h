#ifndef DRAW_H
#define DRAW_H

#include "screen.h"

/* returns x coordinate of first character past the string */
int term_draw_string(int x, int y,
						const char* str, term_char_format fmt);

void term_draw_line(int x0, int y0, int x1, int y1,
						const term_char_t ch, term_char_format fmt);
void term_draw_rect(int x0, int y0, int x1, int y1,
						const term_char_t ch, term_char_format fmt);

#endif
