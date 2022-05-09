#include <stdlib.h>

#include "screen.h"
#include "draw.h"
#include "config.h"
#include "window.h"
#include "ui_elements/button.h"
#include "ui_elements/checkbox.h"
#include "ui_elements/textbox.h"
#include "ui_elements/label.h"
#include "ui_elements/scrollbox.h"
#include "ui_elements/combobox.h"

void wnd_bg_draw()
{
	term_draw_string(1, 2, "Hello world!", (term_char_format){0});
}
void abutton_press(ui_button* self)
{
	static int i = 0;
	if(i % 2 == 0)
		self->parent_wnd->title = "A button. Dear God.";
	else
		self->parent_wnd->title = "There is more. Oh no.";
	++i;

	term_screen_resize(80 + (i % 2) * 80, 40);
}

int main()
{
	term_screen_init(0, 0, 1); // Autoresizing is enabled, so width/height are ignored

	// Parse config
	FILE* conf_fd = fopen("config_example", "r");
	config_error* errs = NULL;
	config_list* conf = config_parse_file(conf_fd, &errs);
	if(errs){
		fprintf(stderr, "Got errors parsing config:\n");
		config_error* errs_it = errs;
		while(errs_it){
			fprintf(stderr, "%s", errs_it->text);
			errs_it = errs_it->next;
		}
		return -1;
	}
	fclose(conf_fd);

	// Search for keybinds in the config and then feed them to te keybind manager
	config_object* cur = NULL;
	config_list* keyconf = NULL;
	while(cur = config_list_next(conf, cur)){
		if(cur->type == CONFIG_TYPE_PROPERTY){
			config_property* prop = (config_property*)cur;
			if(!strcmp(prop->name, "keys") && prop->value && prop->value->type == CONFIG_TYPE_LIST){
				keyconf = (config_list*)prop->value;
				break;
			}
		}
	}
	if(!keyconf){
		fprintf(stderr, "Got errors parsing keybinds:\nCouldn't find \"keys\" property\n");
		return -2;
	}
	errs = input_load_keybind_config(keyconf);
	if(errs){
		fprintf(stderr, "Got errors parsing keybinds:\n");
		config_error* errs_it = errs;
		while(errs_it){
			fprintf(stderr, "%s", errs_it->text);
			errs_it = errs_it->next;
		}
		return -3;
	}

	// Create background window
	window* wnd_bg = malloc(sizeof(window));
	*wnd_bg = (window){.x = 0, .y = 0, .w = 80, .h = 40,
						.title = "background window", .border_style = WINDOW_BORDER_STYLE_BOX_DOUBLE_LINE,
						.on_draw = wnd_bg_draw};

	// Create 1st window
	window* wnd_test = malloc(sizeof(window));
	*wnd_test = (window){.x = 15, .y = 15, .w = 30, .h = 10,
							.title = "test window", .border_style = WINDOW_BORDER_STYLE_BOX_DOUBLE_LINE,
							.can_be_active = 1};
	window_add_ui_element(wnd_test, (ui_element*)ui_button_create(1, 1, "a button", UI_BUTTON_STYLE_BRACKETS, abutton_press));
	window_add_ui_element(wnd_test, (ui_element*)ui_checkbox_create(1, 2, "a checkbox", NULL));
	window_add_ui_element(wnd_test, (ui_element*)ui_textbox_create(1, 3, 10, 128, NULL));
	ui_combobox* combogroup1 = NULL;
	window_add_ui_element(wnd_test, (ui_element*)ui_combobox_create(3, 5, "Linux", &combogroup1, NULL));
	window_add_ui_element(wnd_test, (ui_element*)ui_combobox_create(3, 6, "FreeBSD", &combogroup1, NULL));
	window_add_ui_element(wnd_test, (ui_element*)ui_combobox_create(3, 7, "TempleOS", &combogroup1, NULL));
	window_list_insert_after(wnd_bg, wnd_test);

	// Create 2nd window
	window* wnd_test2 = malloc(sizeof(window));
	*wnd_test2 = (window){.x = 40, .y = 13, .w = 35, .h = 10,
							.title = "test window no. #2", .border_style = WINDOW_BORDER_STYLE_BOX_DOUBLE_LINE,
							.can_be_active = 1};
	window_add_ui_element(wnd_test2, (ui_element*)ui_label_create(1, 1, 28, 3, 1, "This label wraps around it's " BGCLR_RED "width." FORMAT_NORMAL " Crazy, right?"));
	window_add_ui_element(wnd_test2, (ui_element*)ui_scrollbox_create(1, 4, 28, 5, "Same thing, but " FORMAT_UNDERLINE "scrollable" FORMAT_NORMAL ". I called it scrollbox. Why? I don't fucking know. Noone knows. This mystery is beyond human comprehension. This may look like a " FGCLR_BLUE "padding text" FORMAT_NORMAL " to demonstrate how awesome this UI element is, but in fact, it's not the sole purpose of this text. Very few people can actually decipher what this text means."));
	window_list_insert_after(wnd_test, wnd_test2);

	// Set 1st window as initially active
	window_list_set_active(wnd_test);

	// Loop forever, taking a key input, letting currently active window and it's currently active element to process it, then re-rendering the screen
	// You could do better and make separate threads for input/rendering; however, here nothing changes between key presses, so it's fine
	input_init();
	while(1){
		utf_char_t c = {0};
		input_getchar(c);
		window_list_on_input(c);
		window_list_on_draw(wnd_bg);
		term_screen_flush();
	}

	input_restore(); // Not needed, but in case your event loop terminates, don't forget to call this function
}
