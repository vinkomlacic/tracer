#pragma once

/*
 * Defines logging output in the console.
 */
#include <stdarg.h>

#include "con_color.h"


#define OUTPUT_FORMAT "%-6s [%-20s:%-4d]  "


extern ansi_color_t const TRACE_COLOR;
extern bool const TRACE_COLOR_BOLD;
extern ansi_color_t const DEBUG_COLOR;
extern bool const DEBUG_COLOR_BOLD;
extern ansi_color_t const INFO_COLOR;
extern bool const INFO_COLOR_BOLD;
extern ansi_color_t const WARN_COLOR;
extern bool const WARN_COLOR_BOLD;
extern ansi_color_t const ERROR_COLOR;
extern bool const ERROR_COLOR_BOLD;


__attribute__ ((format(printf, 3, 4)))
extern void trace(char const filename[], unsigned line, char const format[], ...);


__attribute__ ((format(printf, 3, 4)))
extern void debug(char const filename[], unsigned line, char const format[], ...);


__attribute__ ((format(printf, 3, 4)))
extern void info(char const filename[], unsigned line, char const format[], ...);


__attribute__ ((format(printf, 3, 4)))
extern void warn(char const filename[], unsigned line, char const format[], ...);


__attribute__ ((format(printf, 1, 2)))
extern void error(char const format[], ...);
