#include "input.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "htable_oa.h"

#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "screen.h"

static struct termios term_old_opts;
static void kbint_sigaction(int sig, siginfo_t* info, void* ucontext)
{
	input_restore();
	exit(0);
}

void input_init()
{
	struct termios tnew;
	tcgetattr(STDIN_FILENO, &term_old_opts);
	tnew = term_old_opts;
	tnew.c_lflag &= ~ICANON;	// enable character input without newline
	tnew.c_lflag &= ~ECHO;		// disable input echoing
	fputs(CMD_HIDE_CURSOR, stdout);
	tcsetattr(STDIN_FILENO, TCSANOW, &tnew);

	// restore terminal settings when the program is terminated
	struct sigaction sigact_old;
	struct sigaction sigact_new = {
		.sa_sigaction = kbint_sigaction
	};
	sigaction(SIGINT, &sigact_new, &sigact_old);
}

void input_restore()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &term_old_opts);
	fputs(CMD_SHOW_CURSOR, stdout);
}


void input_getchar(utf_char_t char_out)
{
	int c = getchar();
	if(c == EOF){
		char_out[0] = '\0';
		return;
	}
	// Handle ANSI escape sequences (only arrows and pageup/pagedown)
	if(c == '\x1b'){
		char_out[0] = c;
		char_out[1] = getchar();
		char_out[2] = getchar();
		if(isdigit(char_out[2])){
			char_out[3] = getchar();
			char_out[4] = '\0';
		}
		else
			char_out[3] = '\0';
		return;
	}
	// Handle UTF-8
	if(c & 128){
		char_out[0] = c;
		size_t i; for(i = 1; i < UTF_CHAR_LENGTH; ++i){
			char_out[i] = getchar();
			if(!(char_out[i] & 64))
			{ ++i; break; }
		}
		char_out[i] = '\0';
	}
	else{
		char_out[0] = c;
		char_out[1] = '\0';
	}
}

void input_utf_char_from_string(const char** str, utf_char_t char_out,
								term_char_format* fmt_out)
{
	while(**str){
		if(**str == '\x1b'){
			++(*str); if(!(**str)) return;
			if(**str != '[') continue;
			++(*str); if(!(**str)) return;
			if((*str)[1] == 'm'){
				if(fmt_out) switch(**str){
					case '0': *fmt_out = (term_char_format){0}; break;
					case '1': fmt_out->flags |= CHAR_FORMAT_BOLD; break;
					case '3': fmt_out->flags |= CHAR_FORMAT_ITALIC; break;
					case '4': fmt_out->flags |= CHAR_FORMAT_UNDERLINE; break;
				}
				*str += 2;
			}
			else if( (*str)[1] && (*str)[2] == 'm'){
				if(fmt_out) switch(**str){
					case '3':
						fmt_out->flags |= CHAR_FORMAT_FGCLR;
						switch((*str)[1]){
							case '0': fmt_out->fg_clr = FGCLR_BLACK; break;
							case '1': fmt_out->fg_clr = FGCLR_RED; break;
							case '2': fmt_out->fg_clr = FGCLR_GREEN; break;
							case '3': fmt_out->fg_clr = FGCLR_YELLOW; break;
							case '4': fmt_out->fg_clr = FGCLR_BLUE; break;
							case '5': fmt_out->fg_clr = FGCLR_MAGENTA; break;
							case '6': fmt_out->fg_clr = FGCLR_CYAN; break;
							case '7': fmt_out->fg_clr = FGCLR_WHITE; break;
						} break;
					case '4':
						fmt_out->flags |= CHAR_FORMAT_BGCLR;
						switch((*str)[1]){
							case '0': fmt_out->bg_clr = BGCLR_BLACK; break;
							case '1': fmt_out->bg_clr = BGCLR_RED; break;
							case '2': fmt_out->bg_clr = BGCLR_GREEN; break;
							case '3': fmt_out->bg_clr = BGCLR_YELLOW; break;
							case '4': fmt_out->bg_clr = BGCLR_BLUE; break;
							case '5': fmt_out->bg_clr = BGCLR_MAGENTA; break;
							case '6': fmt_out->bg_clr = BGCLR_CYAN; break;
							case '7': fmt_out->bg_clr = BGCLR_WHITE; break;
						} break;
				}
				*str += 3;
			}
			else
				++(*str);
		}
		else
			break;
	}

	if(**str & 128){
		char_out[0] = **str;
		size_t i; for(i = 1; i < UTF_CHAR_LENGTH; ++i){
			++(*str);
			char_out[i] = **str;
			if(!(char_out[i] & 64))
			{ ++(*str); ++i; break; }
		}
		char_out[i] = '\0';
	}
	else{
		char_out[0] = **str;
		char_out[1] = '\0';
		++(*str);
	}
}

size_t input_utf_string_length(const char* str)
{
	if(!*str)
		return 0;
	size_t ln = 0;
	while(*str){
		utf_char_t ch;
		input_utf_char_from_string(&str, ch, NULL);
		++ln;
	}
	return ln;
}


/* symbolic key names */

static const char* names[] = {
	"key_up", "key_down", "key_left", "key_right",
	"delete", "backspace", "enter", "spacebar",
	"page_up", "page_down"
};
static const utf_char_t chars[] = {
	"\x1b[A", "\x1b[B", "\x1b[D", "\x1b[C",
	"\x1b[3~", "\x7f", "\n", " ",
	"\x1b[5~", "\x1b[6~"
};

void input_name_to_char(const char* name, utf_char_t char_out)
{
	for(size_t i = 0; i < sizeof(names) / sizeof(*names); ++i)
		if(!strcmp(name, names[i])){
			strcpy(char_out, chars[i]);
			return;
		}
	strncpy(char_out, name, UTF_CHAR_LENGTH);
}
const char* input_char_to_name(const utf_char_t char_in)
{
	for(size_t i = 0; i < sizeof(chars) / sizeof(*chars); ++i)
		if(!strcmp(char_in, chars[i]))
			return names[i];
	return NULL;
}


/* keyboard config */

// key: keyboard key, value: keybind name
DEF_HTABLE_OA(kb, char*, char**);
htable_oa_kb kb_conf;
static size_t kb_conf_hash(size_t table_sz, const char** key)
{
	size_t h = 0; const char* it = *key;
	for(; *it; ++it)
	{ h += *it; h *= 256; }
	return h;
}
int kb_conf_cmp(const char** key1, const char** key2)
{
	return strcmp(*key1, *key2);
}

config_error* input_load_keybind_config(config_list* bind_lst)
{
	config_error* err_out = NULL;

	htable_oa_kb_create(&kb_conf, 16, kb_conf_hash, kb_conf_cmp);
	size_t kb_cnt = 0;
	config_object* cur = NULL;
	while(cur = config_list_next(bind_lst, cur)){
		if(cur->type != CONFIG_TYPE_PROPERTY){
			config_add_error(&err_out, CONFIG_ERROR_ERROR, "Got a non-property object of type %d\n", cur->type);
			continue;
		}
		config_property* _kb = (config_property*)cur;
		if(!_kb->value){
			config_add_error(&err_out, CONFIG_ERROR_ERROR, "Property \"%s\" got no value\n", _kb->name);
			continue;
		}
		if(_kb->value->type != CONFIG_TYPE_STRING){
			config_add_error(&err_out, CONFIG_ERROR_ERROR, "Property \"%s\" has a non-string value\n", _kb->name);
			continue;
		}

		char* utf_char = malloc(UTF_CHAR_LENGTH + 1);
		input_name_to_char(((config_string*)_kb->value)->value, utf_char);
		const char*** binds = htable_oa_kb_find(&kb_conf, utf_char);
		if(!binds){
			char** binds = malloc(sizeof(char*) * 2);
			binds[0] = _kb->name;
			binds[1] = NULL;
			htable_oa_kb_insert(&kb_conf, utf_char, binds);
		}
		else{
			size_t binds_cnt = 1;
			for(const char** it = *binds; *it; ++it)
				++binds_cnt;
			*binds = realloc(*binds, (binds_cnt + 1) * sizeof(char*));
			(*binds)[binds_cnt - 1] = _kb->name;
			(*binds)[binds_cnt] = NULL;
		}
		++kb_cnt;
		if(kb_cnt > kb_conf.size * 2 / 3)
			htable_oa_kb_rebuild(&kb_conf, kb_conf.size * 2);
	}

	return err_out;
}

const char** input_char_to_binds(const utf_char_t char_in)
{
	const char*** _out = htable_oa_kb_find(&kb_conf, char_in);
	static const char* nullstr = NULL;
	return _out ? *_out : &nullstr;
}

int check_bind(const char** binds, const char* bind)
{
	for(; *binds; ++binds)
		if(!strcmp(*binds, bind))
			return 1;
	return 0;
}
