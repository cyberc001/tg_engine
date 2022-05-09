#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

size_t config_object_size[3] = {
	sizeof(config_string), sizeof(config_property), sizeof(config_list)
};

void config_add_error(config_error** error_out, int severity, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char buf[4096];
	snprintf(buf, sizeof(buf), "%s",
				  severity == CONFIG_ERROR_ERROR ? "error: "
				: severity == CONFIG_ERROR_WARNING ? "warning: "
				: "");
	vsnprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), fmt, args);

	config_error* new_err = malloc(sizeof(config_error));
	new_err->next = *error_out;
	new_err->severity = severity;
	new_err->text = strdup(buf);
	*error_out = new_err;

	va_end(args);
}


static char* get_token(char** file, char** pos_out)
{
	static const char seps_begin[] = {' ', '\t', '\n', '\r'};
	static const char seps_end[] = {' ', '\t', '\n', '\r', '=', '{', '}'};

	size_t buf_sz = 16;
	char* buf = malloc(buf_sz);
	size_t buf_i = 0;

	int in_quotes = 0;
	enum {
		TOK_STATE_SKIP,
		TOK_STATE_READ
	} state = TOK_STATE_SKIP;

	for(; **file; ++(*file)){
		switch(state){
			case TOK_STATE_SKIP:
			{
				int should_skip = 0;
				for(size_t i = 0; i < sizeof(seps_begin); ++i)
					if(**file == seps_begin[i])
					{ should_skip = 1; break; }
				if(!should_skip){
					--(*file);
					state = TOK_STATE_READ;
					if(pos_out) *pos_out = *file + 1;
				}
			} break;
			case TOK_STATE_READ:
			{
				if(**file == '\"')
					in_quotes = !in_quotes;
				if(!in_quotes)
					for(size_t i = 0; i < sizeof(seps_end); ++i)
						if(**file == seps_end[i]){
							if(buf_i == 0){
								for(size_t j = 0; j < sizeof(seps_begin); ++j){
									if(seps_end[i] == seps_begin[j]){
										buf[buf_i] = '\0';
										return buf;
									}
								}
							}
							else{
								buf[buf_i] = '\0';
								return buf;
							}
						}
				if(**file != '\"'){
					buf[buf_i++] = **file;
					if(buf_i >= buf_sz){
						buf_sz += 16;
						buf = realloc(buf, buf_sz);
					}
				}
			} break;
		}
	}

	buf[buf_i] = '\0';
	return buf;
}

static config_list* parse_list(char** file, char* end_token, config_error** error_out);
static config_object* parse_object(char** file, config_error** error_out)
{
	while(1){
		char* name_pos;
		char* name = get_token(file, &name_pos);
		if(!strcmp(name, ""))
			return NULL;
		else if(!strcmp(name, "{")){
			return (config_object*)parse_list(file, "}", error_out);
		}

		char* op = get_token(file, NULL);
		if(!strcmp(op, "")){
			config_string* str = malloc(sizeof(config_string));
			str->type = CONFIG_TYPE_STRING;
			str->value = name;
			return (config_object*)str;
		}
		else if(!strcmp(op, "=")){
			config_object* value = parse_object(file, error_out);
			if(!value){
				config_add_error(error_out, CONFIG_ERROR_ERROR, "property \"%s\" does not have a value\n", name);
				return NULL;
			}
			config_property* prop = malloc(sizeof(config_property));
			prop->type = CONFIG_TYPE_PROPERTY;
			prop->name = name;
			prop->value = value;
			return (config_object*)prop;
		}
		else{ // two consecutive strings; rewind to the end of the first one
			*file = name_pos + strlen(name);

			config_string* str = malloc(sizeof(config_string));
			str->type = CONFIG_TYPE_STRING;
			str->value = name;
			return (config_object*)str;
		}
	}
}

static config_list* parse_list(char** file, char* end_token, config_error** error_out)
{
	config_list* lst = malloc(sizeof(config_list));
	lst->type = CONFIG_TYPE_LIST;
	lst->items = NULL; lst->items_sz = 0;

	while(1){
		config_object* obj = parse_object(file, error_out);
		if(!obj || (obj->type == CONFIG_TYPE_STRING && !strcmp(((config_string*)obj)->value, end_token)))
			break;

		config_list_insert(lst, obj);
	}

	return lst;
}

config_list* config_parse_file(FILE* fd, config_error** error_out)
{
	fseek(fd, 0, SEEK_END);
	long fsz = ftell(fd);
	rewind(fd);

	char* file = malloc(fsz + 1);
	file[fsz] = '\0';
	fread(file, 1, fsz, fd);

	char* file_it = file;
	config_list* lst = parse_list(&file_it, "", error_out);

	free(file);
	return lst;
}


void config_list_insert(config_list* lst, config_object* item)
{
	size_t item_size = config_object_size[item->type];
	lst->items_sz += item_size;
	lst->items = realloc(lst->items, lst->items_sz);

	memcpy((void*)lst->items + (lst->items_sz - item_size), item, item_size);
}

config_object* config_list_next(config_list* lst, config_object* cur)
{
	if(!cur)
		return lst->items;
	size_t sz = config_object_size[cur->type];
	cur = (void*)cur + sz;
	if((void*)cur >= (void*)lst->items + lst->items_sz)
		return NULL;
	return cur;
}


static void print_config_object_r(config_object* obj, unsigned depth)
{
	for(unsigned i = 0; i < depth; ++i) printf("  ");
	switch(obj->type){
		case CONFIG_TYPE_STRING:
			printf("string: \"%s\"\n", ((config_string*)obj)->value);
			break;
		case CONFIG_TYPE_PROPERTY:
			printf("property: \"%s\":\n", ((config_property*)obj)->name);
			print_config_object_r(((config_property*)obj)->value, depth);
			break;
		case CONFIG_TYPE_LIST:
			printf("list: \n");
			config_object* cur = NULL;
			while(cur = config_list_next((config_list*)obj, cur))
				print_config_object_r(cur, depth + 1);
			break;
	}
}
void print_config_object(config_object* obj)
{ print_config_object_r(obj, 0); }
