#ifndef WINDOW_H
#define WINDOW_H

#include "input.h"

#define WINDOW_BORDER_STYLE_NONE				0
#define WINDOW_BORDER_STYLE_BOX_DOUBLE_LINE		1

typedef struct ui_element ui_element;

typedef struct window window;
struct window{
	size_t x, y;
	size_t w, h;

	int border_style;
	const char* title;

	int can_be_active;

	/* Return non-0 if the input should be passed further (to UI elements) */
	int (*on_input)(window* self, utf_char_t char_in);
	void (*on_draw)(window* self);

	enum{
		WINDOW_MODE_NORMAL = 0,
		WINDOW_MODE_MOVE,
		WINDOW_MODE_RESIZE
	} mode;

	struct window *next, *prev;

	ui_element *elem_first, *elem_last;
	ui_element* active_elem;
};


void window_list_insert_after(window* after, window* new_wnd);
void window_list_delete(window* to_delete);

/* Adds an UI element to the end of the list of elements */
void window_add_ui_element(window* wnd, ui_element* elem);
/* Removes an UI element from the list of elements */
void window_remove_ui_element(window* wnd, ui_element* elem);

void window_list_set_active(window* active);
window* window_list_get_active();

/* Called automatically (with right number) by window_list_on_draw() */
void window_draw_border(window* wnd, unsigned number);

/* Requires an active window to actually provide input.
*  Handles changing focus using next_window\prev_window keybinds.
*/
void window_list_on_input(utf_char_t char_in);
/* Calls on_draw callback in order of provided linked list,
*  odrawing the active window last (if there is one).
*/
void window_list_on_draw(window* head);

#endif
