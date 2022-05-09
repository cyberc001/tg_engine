#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>
#include <stdio.h>

typedef struct config_object config_object;
struct config_object{
	enum {
		CONFIG_TYPE_STRING,
		CONFIG_TYPE_PROPERTY,
		CONFIG_TYPE_LIST
	} type;
};

typedef struct config_string config_string;
struct config_string{
	struct config_object;
	char* value;
};

typedef struct config_property config_property;
struct config_property{
	struct config_object;
	char* name;
	config_object* value;
};

typedef struct config_list config_list;
struct config_list{
	struct config_object;
	config_object* items;
	size_t items_sz;
};
extern size_t config_object_size[3];
/* Inserts an object into a configuration list.
   Increments items_sz by the size of the said object, found in config_object_size.
*/
void config_list_insert(config_list* lst, config_object* item);
config_object* config_list_next(config_list* lst, config_object* cur);


typedef struct config_error config_error;
struct config_error{
	enum {
		CONFIG_ERROR_ERROR,
		CONFIG_ERROR_WARNING
	} severity;
	char* text;

	struct config_error* next;
};

/* Parses a configuration file, giving a list of configuration objects.
*  Arguments:
*	fd - file descriptor, managed outside of the function, reading pointer should be at 0
*	error_out - pointer to a linked list of errors. What is points to should be either a valid config_error structure or NULL.
*  Return values:
*	Returns a list of config objects, allocated on heap.
*	<error_out> Appends errors that happened during parsing (nothing changes if no errors were encountered)
*/
config_list* config_parse_file(FILE* fd, config_error** error_out);
/* Adds an error to the front linked list.
*  What error_out points to can be NULL (if the linked list is empty).
*/
void config_add_error(config_error** error_out, int severity, const char* fmt, ...);

/* Prints a configuration object, depending on it's type. */
void print_config_object(config_object* obj);

#endif
