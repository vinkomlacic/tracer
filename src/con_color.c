#include <stdio.h>

#include "t_error.h"

#include "con_color.h"


extern void set_console_color(ansi_color_t const color, bool const bold) {
    set_console_color_os(stdout, color, bold);
}


extern void reset_console_color(void) {
    reset_console_color_os(stdout);
}


extern void set_console_color_os(FILE * const file, ansi_color_t const color, bool const bold) {
    if (file == NULL) {
        raise(T_ENULL_ARG, "file");
        return;
    }

    switch (color) {
        case BLACK:
            fprintf(file, "\x1B[%d;30m", bold ? 1 : 0);
            break;
        case RED:
            fprintf(file, "\x1B[%d;31m", bold ? 1 : 0);
            break;
        case GREEN:
            fprintf(file, "\x1B[%d;32m", bold ? 1 : 0);
            break;
        case YELLOW:
            fprintf(file, "\x1B[%d;33m", bold ? 1 : 0);
            break;
        case BLUE:
            fprintf(file, "\x1B[%d;34m", bold ? 1 : 0);
            break;
        case MAGENTA:
            fprintf(file, "\x1B[%d;35m", bold ? 1 : 0);
            break;
        case CYAN:
            fprintf(file, "\x1B[%d;36m", bold ? 1 : 0);
            break;
        case WHITE:
            fprintf(file, "\x1B[%d;37m", bold ? 1 : 0);
            break;
        default:
            raise(T_EUNKNOWN_COLOR, "%d", color);
    }
}


extern void reset_console_color_os(FILE * const file) {
    if (file == NULL) {
        raise(T_ENULL_ARG, "file");
        return;
    }

    fprintf(file, "\033[0m");
}