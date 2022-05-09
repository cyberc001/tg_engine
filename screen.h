#ifndef SCREEN_H
#define SCREEN_H

#include "format.h"
#include "input.h"

#include <stddef.h>
#include <string.h>

#define SCREEN_CHAROFF_NORMAL		0 	// 4
#define SCREEN_CHAROFF_BOLD			4 	// 4
#define SCREEN_CHAROFF_ITALIC		8	// 4
#define SCREEN_CHAROFF_UNDERLINE	12	// 4
#define SCREEN_CHAROFF_FGCLR		16 	// 5
#define SCREEN_CHAROFF_BGCLR		21 	// 5

#define SCREEN_FORMAT_SIZE			26

typedef char term_char_t[SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH];

struct {
	size_t w, h;
	int autoresize;

	term_char_t* buffer;
	term_char_t* second_buffer;

	struct{
		int x0, y0;
		int x1, y1;
	} bound_box;
} term_screen;


#define CHAR_FORMAT_BOLD		0b00001
#define CHAR_FORMAT_ITALIC		0b00010
#define CHAR_FORMAT_UNDERLINE	0b00100
#define CHAR_FORMAT_FGCLR		0b01000
#define CHAR_FORMAT_BGCLR		0b10000

typedef struct term_char_format term_char_format;
struct term_char_format{
	int flags;
	const char* fg_clr;
	const char* bg_clr;
};

/* Tries to add "addon" to the "orig"inal format.
*  Basically binary ORs flags and adds missing foreground/background colors.
*/
void merge_formats(term_char_format* orig, const term_char_format* addon);

/* Initializes the terminal screen.
*  If autoresize is true, terminal size is autodetected and autoresize is enabled.
*/
void term_screen_init(size_t w, size_t h, int autoresize);
void term_screen_resize(size_t w, size_t h);
/* Prints screen buffer in terminal */
void term_screen_flush();

/* Basic character manipulation: */
#define term_getchar(x, y) (term_screen.buffer[(x) + (y) * (term_screen.w + 1)])
#define term_setchar(x, y, ch)\
{\
	if(is_point_in_bounding_box(x, y)){\
		memset(term_getchar(x, y) + SCREEN_FORMAT_SIZE, 0, UTF_CHAR_LENGTH);\
		memcpy(term_getchar(x, y) + SCREEN_FORMAT_SIZE, (ch), strlen(ch));\
	}\
}

void term_setformat_raw(int x, int y, term_char_format fmt);
#define term_setformat(x, y, ...)\
{ /*bounding box check is done in term_setformat_raw()*/\
	term_char_format f = (term_char_format){__VA_ARGS__};\
	term_setformat_raw(x, y, f);\
}

/* Global bounding box state: */
void term_set_bounding_box(int x0, int y0, int x1, int y1);
void term_reset_bounding_box();
int is_point_in_bounding_box(int x, int y);

#endif
