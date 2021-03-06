#pragma once

/*
 * Allows changing the color of the output.
 */

#include <stdio.h>
#include <stdbool.h>


typedef enum {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
} ansi_color_t;


/**
 * Sets the console color output to the specified ANSI color.
 * You can either choose bold or not bold version.
 */
extern void set_console_color(ansi_color_t color, bool bold);


/**
 * Resets the console output color.
 */
extern void reset_console_color(void);


/**
 * Same as set_console_color, only it allows specifying a stream where to set the color.
 * If file is null, T_ENULL_ARG is raised.
 */
extern void set_console_color_os(FILE * file, ansi_color_t color, bool bold);


/**
 * Same as reset_console_color, only it allows specifying a stream where to set the color.
 * If file is null, T_ENULL_ARG is raised.
 */
extern void reset_console_color_os(FILE * file);
