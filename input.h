#ifndef INPUT_H
#define INPUT_H

#include "config.h"

typedef struct term_char_format term_char_format;

#define UTF_CHAR_LENGTH			4
typedef char utf_char_t[UTF_CHAR_LENGTH + 1];

/* Initializes input (clears ICANON, eliminating input on newline) */
void input_init();
/* Restores input settings (like cursor visibility)*/
void input_restore();

/* Gets a single "character", it being either a UTF-8 codepoint or an ANSI escape sequence. */
void input_getchar(utf_char_t char_out);
/* Extracts a UTF-8 codepoint from a string, changing the pointer.
Also changes supplied format (if it's non-NULL, of course). */
void input_utf_char_from_string(const char** str, utf_char_t char_out,
								term_char_format* fmt_out);
size_t input_utf_string_length(const char* str);

/* symbolic key names */
void input_name_to_char(const char* name, utf_char_t char_out);
const char* input_char_to_name(const utf_char_t char_in);

/* keybind config */
config_error* input_load_keybind_config(config_list* bind_lst);
const char** input_char_to_binds(const utf_char_t char_in);
int check_bind(const char** binds, const char* bind);

#endif
