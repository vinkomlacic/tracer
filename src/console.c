#include "console.h"

ansi_color_t const TRACE_COLOR = CYAN;
bool const TRACE_COLOR_BOLD = false;
ansi_color_t const DEBUG_COLOR = GREEN;
bool const DEBUG_COLOR_BOLD = false;
ansi_color_t const INFO_COLOR = WHITE;
bool const INFO_COLOR_BOLD = true;
ansi_color_t const WARN_COLOR = YELLOW;
bool const WARN_COLOR_BOLD = false;
ansi_color_t const ERROR_COLOR = RED;
bool const ERROR_COLOR_BOLD = false;


__attribute__ ((format(printf, 3, 4)))
extern void trace(char const filename[const], unsigned const line, char const format[const], ...) {
    va_list arguments;
    va_start(arguments, format);

    set_console_color(TRACE_COLOR, TRACE_COLOR_BOLD);
    printf("%-6s [%-20s:%-4d]  ", "TRACE", filename, line);
    vprintf(format, arguments);
    printf("\n");
    reset_console_color();

    va_end(arguments);
}


__attribute__ ((format(printf, 3, 4)))
extern void debug(char const filename[const], unsigned const line, char const format[const], ...) {
    va_list arguments;
    va_start(arguments, format);

    set_console_color(DEBUG_COLOR, DEBUG_COLOR_BOLD);
    printf("%-6s [%-20s:%-4d]  ", "DEBUG", filename, line);
    vprintf(format, arguments);
    printf("\n");
    reset_console_color();

    va_end(arguments);
}


__attribute__ ((format(printf, 3, 4)))
extern void info(char const filename[const], unsigned const line, char const format[const], ...) {
    va_list arguments;
    va_start(arguments, format);

    set_console_color(INFO_COLOR, INFO_COLOR_BOLD);
    printf("%-6s [%-20s:%-4d]  ", "INFO", filename, line);
    vprintf(format, arguments);
    printf("\n");
    reset_console_color();

    va_end(arguments);
}


__attribute__ ((format(printf, 3, 4)))
extern void warn(char const filename[const], unsigned const line, char const format[const], ...) {
    va_list arguments;
    va_start(arguments, format);

    set_console_color(WARN_COLOR, WARN_COLOR_BOLD);
    printf("%-6s [%-20s:%-4d]  ", "WARN", filename, line);
    vprintf(format, arguments);
    printf("\n");
    reset_console_color();

    va_end(arguments);
}


__attribute__ ((format(printf, 3, 4)))
extern void error(char const filename[const], unsigned const line, char const format[const], ...) {
    va_list arguments;
    va_start(arguments, format);

    set_console_color_os(stderr, ERROR_COLOR, ERROR_COLOR_BOLD);
    fprintf(stderr, "%-6s [%-20s:%-4d]  ", "ERROR", filename, line);
    vfprintf(stderr, format, arguments);
    fprintf(stderr, "\n");
    reset_console_color_os(stderr);

    va_end(arguments);
}
