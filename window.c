#include "window.h"

#include <string.h>
#include "screen.h"
#include "draw.h"
#include "ui_element.h"

static window* wnd_active = NULL;


void window_list_insert_after(window* after, window* new_wnd)
{
	if(after->next){
		after->next->prev = new_wnd;
		new_wnd->next = after->next;
	}
	after->next = new_wnd;
	new_wnd->prev = after;
}

void window_list_delete(window* to_delete)
{
	if(to_delete == wnd_active){
		if(to_delete->prev)
			wnd_active = to_delete->prev;
		else
			wnd_active = to_delete->next;
	}
	if(to_delete->prev)
		to_delete->prev->next = to_delete->next;
	if(to_delete->next)
		to_delete->next->prev = to_delete->prev;
}

void window_add_ui_element(window* wnd, ui_element* elem)
{
	if(!wnd->elem_first){
		wnd->elem_first = wnd->elem_last = elem;
	}
	else{
		wnd->elem_last->next = elem;
		elem->prev = wnd->elem_last;
		wnd->elem_last = elem;
	}
	elem->parent_wnd = wnd;
}

void window_remove_ui_element(window* wnd, ui_element* elem)
{
	if(wnd->elem_first == elem)
		wnd->elem_first = elem->next;
	if(wnd->elem_last == elem)
		wnd->elem_last = elem->prev;
	if(elem->prev)
		elem->prev->next = elem->next;
	if(elem->next)
		elem->next->prev = elem->prev;
	if(wnd->active_elem == elem)
		wnd->active_elem = elem->prev ? elem->prev : elem->next;
}


void window_list_set_active(window* active) { wnd_active = active; }
window* window_list_get_active() { return wnd_active; }


void window_draw_border(window* wnd, unsigned number)
{
	term_draw_rect(wnd->x, wnd->y, wnd->x + wnd->w - 1, wnd->y + wnd->h - 1, " ", (term_char_format){0});

	switch(wnd->border_style){
		case WINDOW_BORDER_STYLE_BOX_DOUBLE_LINE:
		{
			term_char_format border_fmt;
			if(wnd == wnd_active)
				border_fmt = (term_char_format){.flags = CHAR_FORMAT_ITALIC};
			else
				border_fmt = (term_char_format){0};

			term_setchar(wnd->x, wnd->y, "╔");
			term_setformat_raw(wnd->x, wnd->y, border_fmt);
			term_draw_line(wnd->x + 1, wnd->y, wnd->x + wnd->w - 2, wnd->y, "═", border_fmt);
			term_setchar(wnd->x + wnd->w - 1, wnd->y, "╗");
			term_setformat_raw(wnd->x + wnd->w - 1, wnd->y, border_fmt);
			term_draw_line(wnd->x + wnd->w - 1, wnd->y + 1, wnd->x + wnd->w - 1, wnd->y + wnd->h - 2, "║", border_fmt);
			term_setchar(wnd->x + wnd->w - 1, wnd->y + wnd->h - 1, "╝");
			term_setformat_raw(wnd->x + wnd->w - 1, wnd->y + wnd->h - 1, border_fmt);
			term_draw_line(wnd->x + wnd->w - 2, wnd->y + wnd->h - 1, wnd->x + 1, wnd->y + wnd->h - 1, "═", border_fmt);
			term_setchar(wnd->x, wnd->y + wnd->h - 1, "╚");
			term_setformat_raw(wnd->x, wnd->y + wnd->h - 1, border_fmt);
			term_draw_line(wnd->x, wnd->y + wnd->h - 2, wnd->x, wnd->y + 1, "║", border_fmt);

			char wnd_num_str[16];
			snprintf(wnd_num_str, sizeof(wnd_num_str), "%u", number);
			int x = term_draw_string(wnd->x + 1, wnd->y, wnd_num_str, (term_char_format){0});
			const char* mode_str = wnd->mode == WINDOW_MODE_MOVE 	? "<move>"
								 : wnd->mode == WINDOW_MODE_RESIZE	? "<resize>"
								 : "";
			x = term_draw_string(x + 1, wnd->y, mode_str, (term_char_format){0});
			x = term_draw_string(x + 1, wnd->y, wnd->title, (term_char_format){0});
		} break;
	}
}


void window_list_on_input(utf_char_t char_in)
{
	if(!wnd_active)
		return;

	switch(wnd_active->mode){
		case WINDOW_MODE_NORMAL:
		{
			const char** binds = input_char_to_binds(char_in);
			if(check_bind(binds, "window_change_mode")){
				wnd_active->mode = WINDOW_MODE_MOVE;
			}
			else if(check_bind(binds, "next_window")){
				window* nxt = wnd_active->next;
				while(nxt){
					if(nxt->flags & WINDOW_FLAG_CAN_BE_ACTIVE)
					{ window_list_set_active(nxt); break; }
					nxt = nxt->next;
				}
			}
			else if(check_bind(binds, "prev_window")){
				window* prv = wnd_active->prev;
				while(prv){
					if(prv->flags & WINDOW_FLAG_CAN_BE_ACTIVE)
					{ window_list_set_active(prv); break; }
					prv = prv->prev;
				}
			}
			else if(check_bind(binds, "next_ui_element")){
				if(!wnd_active->active_elem){
					ui_element* nxt = wnd_active->elem_first;
					while(nxt){
						if(nxt->flags & UI_ELEMENT_FLAG_CAN_BE_ACTIVE)
						{ wnd_active->active_elem = nxt; break; }
						nxt = nxt->next;
					}
				}
				else{
					ui_element* nxt = wnd_active->active_elem->next;
					while(nxt){
						if(nxt->flags & UI_ELEMENT_FLAG_CAN_BE_ACTIVE)
						{ wnd_active->active_elem = nxt; break; }
						nxt = nxt->next;
					}
				}
			}
			else if(check_bind(binds, "prev_ui_element")){
				if(!wnd_active->active_elem){
					ui_element* prv = wnd_active->elem_last;
					while(prv){
						if(prv->flags & UI_ELEMENT_FLAG_CAN_BE_ACTIVE)
						{ wnd_active->active_elem = prv; break; }
						prv = prv->prev;
					}
				}
				else{
					ui_element* prv = wnd_active->active_elem->prev;
					while(prv){
						if(prv->flags & UI_ELEMENT_FLAG_CAN_BE_ACTIVE)
						{ wnd_active->active_elem = prv; break; }
						prv = prv->next;
					}
				}
			}
			int pass_to_elems = 1;
			if(wnd_active && wnd_active->on_input)
				pass_to_elems = wnd_active->on_input(wnd_active, char_in);
			if(pass_to_elems){
				if(wnd_active->active_elem)
					if(wnd_active->active_elem->on_input)
						wnd_active->active_elem->on_input(wnd_active->active_elem, char_in);
			}
		} break;
		case WINDOW_MODE_MOVE:
		{
			const char** binds = input_char_to_binds(char_in);
			if(check_bind(binds, "window_change_mode")){
				wnd_active->mode = WINDOW_MODE_RESIZE;
			}
			else if(check_bind(binds, "window_move_left")){
				if(wnd_active->x > 0)
					--wnd_active->x;
			}
			else if(check_bind(binds, "window_move_right")){
				if(wnd_active->x < term_screen.w - 2)
					++wnd_active->x;
			}
			else if(check_bind(binds, "window_move_up")){
				if(wnd_active->y > 0)
					--wnd_active->y;
			}
			else if(check_bind(binds, "window_move_down")){
				if(wnd_active->y < term_screen.h - 1)
					++wnd_active->y;
			}
		} break;
		case WINDOW_MODE_RESIZE:
		{
			const char** binds = input_char_to_binds(char_in);
			if(check_bind(binds, "window_change_mode")){
				wnd_active->mode = WINDOW_MODE_NORMAL;
			}
			else if(check_bind(binds, "window_grow_w")){
				if(wnd_active->x + wnd_active->w < term_screen.w - 1)
					++wnd_active->w;
			}
			else if(check_bind(binds, "window_shrink_w")){
				if(wnd_active->w > 0)
					--wnd_active->w;
			}
			else if(check_bind(binds, "window_grow_h")){
				if(wnd_active->y + wnd_active->h < term_screen.h - 1)
					++wnd_active->h;
			}
			else if(check_bind(binds, "window_shrink_h")){
				if(wnd_active->h > 0)
					--wnd_active->h;
			}
			else if(check_bind(binds, "window_resize_to_fit")){
				size_t max_w = input_utf_string_length(wnd_active->title) + 14, max_h = 3;
				ui_element* cur = wnd_active->elem_first;
				while(cur){
					if(cur->x + cur->w + 1 > max_w)
						max_w = cur->x + cur->w + 1;
					if(cur->y + cur->h + 1 > max_h)
						max_h = cur->y + cur->h + 1;
					cur = cur->next;
				}
				wnd_active->w = max_w;
				wnd_active->h = max_h;
			}
		} break;
	}
}

void window_list_on_draw(window* head)
{
	unsigned wnd_no = 0, active_no = 0;
	while(head){
		if(head != wnd_active || head->flags & WINDOW_FLAG_RENDER_ON_BOTTOM){
			term_set_bounding_box(head->x, head->y, head->x + head->w - 1, head->y + head->h - 1);
			window_draw_border(head, wnd_no);
			ui_element* cur = head->elem_first;
			while(cur){
				term_draw_rect(cur->x + head->x, cur->y + head->y, cur->x + head->x + cur->w - 1, cur->y + head->y + cur->h - 1, " ", (term_char_format){0});
				if(cur->on_draw) cur->on_draw(cur);
				cur = cur->next;
			}
			if(head->on_draw) head->on_draw(head);
		}
		else
			active_no = wnd_no;
		++wnd_no;
		head = head->next;
	}

	if(wnd_active && !(wnd_active->flags & WINDOW_FLAG_RENDER_ON_BOTTOM)){
		term_set_bounding_box(wnd_active->x, wnd_active->y, wnd_active->x + wnd_active->w - 1, wnd_active->y + wnd_active->h - 1);
		window_draw_border(wnd_active, active_no);
		ui_element* cur = wnd_active->elem_first;
		while(cur){
			term_draw_rect(cur->x + wnd_active->x, cur->y + wnd_active->y, cur->x + wnd_active->x + cur->w - 1, cur->y + wnd_active->y + cur->h - 1, " ", (term_char_format){0});
			if(cur->on_draw) cur->on_draw(cur);
			cur = cur->next;
		}
		if(wnd_active->on_draw) wnd_active->on_draw(wnd_active);
	}
	term_reset_bounding_box();
}

