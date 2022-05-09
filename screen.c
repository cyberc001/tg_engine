#include "screen.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

void merge_formats(term_char_format* orig, const term_char_format* addon)
{
	orig->flags |= addon->flags;
	if(!orig->fg_clr)
		orig->fg_clr = addon->fg_clr;
	if(!orig->bg_clr)
		orig->bg_clr = addon->bg_clr;
}

static void term_resize_sigaction(int sig, siginfo_t* info, void* ucontext)
{
	struct winsize sz;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz);
	size_t w = sz.ws_col, h = sz.ws_row;
	if(w) --w; if(h) --h;
	term_screen_resize(w, h);
	term_screen_flush();
}

void term_screen_init(size_t w, size_t h, int autoresize)
{
	fwrite(CMD_FLUSH, 1, strlen(CMD_FLUSH), stdout);

	term_screen.autoresize = autoresize;
	if(autoresize){
		struct winsize sz;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz);
		w = sz.ws_col; h = sz.ws_row;
		if(w) --w; if(h) --h;

		struct sigaction sigact_old;
		struct sigaction sigact_new = {
			.sa_sigaction = term_resize_sigaction
		};
		// terminal resize signal
		sigaction(SIGWINCH, &sigact_new, &sigact_old);
	}

	term_screen.w = w; term_screen.h = h;
	term_screen.buffer = malloc(sizeof(term_char_t) * (w + 1) * h);
	term_screen.second_buffer = malloc(sizeof(term_char_t) * (w + 1) * h);
	term_reset_bounding_box();

	for(size_t y = 0; y < term_screen.h; ++y){
		for(size_t x = 0; x < term_screen.w; ++x){
			memset(term_getchar(x, y), 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
			memcpy(term_getchar(x, y), FORMAT_NORMAL, 4);
			term_setchar(x, y, " ");
			memset(term_screen.second_buffer[(x) + (y) * (term_screen.w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
			memcpy(term_screen.second_buffer[(x) + (y) * (term_screen.w + 1)], FORMAT_NORMAL, 4);
			strcpy(term_screen.second_buffer[(x) + (y) * (term_screen.w + 1)] + SCREEN_FORMAT_SIZE, " ");
		}
		memset(term_getchar(w, y), 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
		memcpy(term_getchar(w, y), FORMAT_NORMAL, 4);
		strcpy(term_getchar(w, y) + SCREEN_FORMAT_SIZE, "\n");
		memset(term_screen.second_buffer[(w) + (y) * (term_screen.w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
		memcpy(term_screen.second_buffer[(w) + (y) * (term_screen.w + 1)], FORMAT_NORMAL, 4);
		strcpy(term_screen.second_buffer[(w) + (y) * (term_screen.w + 1)] + SCREEN_FORMAT_SIZE, "\n");
	}
}

void term_screen_resize(size_t w, size_t h)
{
	size_t min_w = w < term_screen.w ? w : term_screen.w;
	size_t min_h = h < term_screen.h ? h : term_screen.h;
	term_char_t* new_buffer = malloc(sizeof(term_char_t) * (w + 1) * h);

	for(size_t y = 0; y < min_h; ++y){
		memcpy(new_buffer[y * (w + 1)], term_screen.buffer, min_w * sizeof(term_char_t));
		if(w > term_screen.w){ // if new width is greater, pad with spaces
			for(size_t x = term_screen.w; x < w; ++x){
				memset(new_buffer[x + y * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
				memcpy(new_buffer[x + y * (w + 1)], FORMAT_NORMAL, 4);
				term_setchar(x, y, " ");
			}
		}
		memset(new_buffer[w + y * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
		memcpy(new_buffer[w + y * (w + 1)], FORMAT_NORMAL, 4);
		strcpy(new_buffer[w + y * (w + 1)] + SCREEN_FORMAT_SIZE, "\n");
	}
	if(h > term_screen.h){ // if new height is greater, pad with spaces
		for(size_t y = term_screen.h; y < h; ++y){
			for(size_t x = 0; x < w; ++x){
				memset(new_buffer[x + y * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
				memcpy(new_buffer[x + y * (w + 1)], FORMAT_NORMAL, 4);
				term_setchar(x, y, " ");
			}
			memset(new_buffer[w + y * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
			memcpy(new_buffer[w + y * (w + 1)], FORMAT_NORMAL, 4);
			strcpy(new_buffer[w + y * (w + 1)] + SCREEN_FORMAT_SIZE, "\n");
		}
	}

	// leave secondary buffer empty
	free(term_screen.second_buffer);
	term_screen.second_buffer = malloc(sizeof(term_char_t) * (w + 1) * h);
	for(size_t y = 0; y < h; ++y){
		for(size_t x = 0; x < w; ++x){
			memset(term_screen.second_buffer[(x) + (y) * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
			memcpy(term_screen.second_buffer[(x) + (y) * (w + 1)], FORMAT_NORMAL, 4);
			strcpy(term_screen.second_buffer[(x) + (y) * (w + 1)] + SCREEN_FORMAT_SIZE, " ");
		}
		memset(term_screen.second_buffer[(w) + (y) * (w + 1)], 0, SCREEN_FORMAT_SIZE + UTF_CHAR_LENGTH);
		memcpy(term_screen.second_buffer[(w) + (y) * (w + 1)], FORMAT_NORMAL, 4);
		strcpy(term_screen.second_buffer[(w) + (y) * (w + 1)] + SCREEN_FORMAT_SIZE, "\n");
	}

	free(term_screen.buffer);
	term_screen.w = w; term_screen.h = h;
	term_screen.buffer = new_buffer;

	term_reset_bounding_box();
	fwrite(CMD_FLUSH, 1, strlen(CMD_FLUSH), stdout);
}

void term_screen_flush()
{
	for(size_t y = 0; y < term_screen.h; ++y){
		if(memcmp(term_screen.buffer[y * (term_screen.w + 1)], term_screen.second_buffer[y * (term_screen.w + 1)], sizeof(term_char_t) * (term_screen.w + 1))){
			printf("\x1b[%lu;H", y + 1);
			fwrite(term_screen.buffer[y * (term_screen.w + 1)], sizeof(term_char_t), term_screen.w + 1, stdout);
		}
	};
	term_char_t* tmp = term_screen.buffer;
	term_screen.buffer = term_screen.second_buffer;
	term_screen.second_buffer = tmp;
}


void term_setformat_raw(int x, int y, term_char_format fmt)
{
	if(!is_point_in_bounding_box(x, y))
		return;
	if(fmt.flags & CHAR_FORMAT_BOLD)		memcpy(term_getchar(x, y) + SCREEN_CHAROFF_BOLD, FORMAT_BOLD, 4);
	else									memset(term_getchar(x, y) + SCREEN_CHAROFF_BOLD, 0, 4);
	if(fmt.flags & CHAR_FORMAT_ITALIC)		memcpy(term_getchar(x, y) + SCREEN_CHAROFF_ITALIC, FORMAT_ITALIC, 4);
	else									memset(term_getchar(x, y) + SCREEN_CHAROFF_ITALIC, 0, 4);
	if(fmt.flags & CHAR_FORMAT_UNDERLINE)	memcpy(term_getchar(x, y) + SCREEN_CHAROFF_UNDERLINE, FORMAT_UNDERLINE, 4);
	else									memset(term_getchar(x, y) + SCREEN_CHAROFF_UNDERLINE, 0, 4);
	if(fmt.flags & CHAR_FORMAT_FGCLR)		memcpy(term_getchar(x, y) + SCREEN_CHAROFF_FGCLR, fmt.fg_clr, 5);
	else									memset(term_getchar(x, y) + SCREEN_CHAROFF_FGCLR, 0, 5);
	if(fmt.flags & CHAR_FORMAT_BGCLR)		memcpy(term_getchar(x, y) + SCREEN_CHAROFF_BGCLR, fmt.bg_clr, 5);
	else									memset(term_getchar(x, y) + SCREEN_CHAROFF_BGCLR, 0, 5);
}

/* Global bounding box state: */
void term_set_bounding_box(int x0, int y0, int x1, int y1)
{
	term_screen.bound_box.x0 = x0 < 0 ? 0
							 : x0 >= term_screen.w ? term_screen.w - 1
							 : x0;
	term_screen.bound_box.y0 = y0 < 0 ? 0
							 : y0 >= term_screen.h ? term_screen.h - 1
							 : y0;
	term_screen.bound_box.x1 = x1 < x0 ? x0
							 : x1 >= term_screen.w ? term_screen.w - 1
							 : x1;
	term_screen.bound_box.y1 = y1 < y0 ? y0
							 : y1 >= term_screen.h ? term_screen.h - 1
							 : y1;
}
void term_reset_bounding_box()
{
	term_screen.bound_box.x0 = 0; term_screen.bound_box.y0 = 0;
	term_screen.bound_box.x1 = term_screen.w - 1; term_screen.bound_box.y1 = term_screen.h - 1;
}

int is_point_in_bounding_box(int x, int y)
{
	return x >= term_screen.bound_box.x0 && y >= term_screen.bound_box.y0
		&& x <= term_screen.bound_box.x1 && y <= term_screen.bound_box.y1;
}
