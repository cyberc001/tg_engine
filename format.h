#ifndef FORMAT_H
#define FORMAT_H

#define FORMAT_NORMAL		"\x1b[0m"
#define FORMAT_BOLD			"\x1b[1m"
#define FORMAT_ITALIC		"\x1b[3m"
#define FORMAT_UNDERLINE	"\x1b[4m"

#define FGCLR_BLACK			"\x1b[30m"
#define FGCLR_RED			"\x1b[31m"
#define FGCLR_GREEN			"\x1b[32m"
#define FGCLR_YELLOW		"\x1b[33m"
#define FGCLR_BLUE			"\x1b[34m"
#define FGCLR_MAGENTA		"\x1b[35m"
#define FGCLR_CYAN			"\x1b[36m"
#define FGCLR_WHITE			"\x1b[37m"

#define BGCLR_BLACK			"\x1b[40m"
#define BGCLR_RED			"\x1b[41m"
#define BGCLR_GREEN			"\x1b[42m"
#define BGCLR_YELLOW		"\x1b[43m"
#define BGCLR_BLUE			"\x1b[44m"
#define BGCLR_MAGENTA		"\x1b[45m"
#define BGCLR_CYAN			"\x1b[46m"
#define BGCLR_WHITE			"\x1b[47m"

#define CMD_FLUSH			"\x1b[1;1H\x1b[2J"
#define CMD_HIDE_CURSOR		"\x1b[?25l"
#define CMD_SHOW_CURSOR		"\x1b[?25h"

#endif
