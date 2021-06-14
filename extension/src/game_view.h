#include <stdio.h>
#include "curses.h"

#define DISTANCE_OUTSIDE 3

char *view[] = {
"#######################################",
"# . . . . . . . . ### . . . . . . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . . . . . . . . . . . . . . . . . #",
"# . ### . # . ########### . # . ### . #",
"# . . . . # . . . ### . . . # . . . . #",
"####### . ##### . ### . ##### . #######",
"      # . # . . . . . . . . # . #      ",
"      # . # . ### - - ### . # . #      ",
"####### . # . #         # . # . #######",
"        . . . #         # . . .        ",
"####### . # . #         # . # . #######",
"      # . # . ########### . # . #      ",
"      # . # . . . .X. . . . # . #      ",
"####### . # . ########### . # . #######",
"# . . . . . . . . ### . . . . . . . . #",
"# O ### . ##### . ### . ##### . ### O #",
"# . . # . . . . . . . . . . . . # . . #",
"### . # . # . ########### . # . # . ###",
"# . . . . # . . . ### . . . # . . . . #",
"# . ########### . ### . ########### . #",
"# . . . . . . . . . . . . . . . . . . #",
"#######################################",                        
    NULL
};

void print_view(WINDOW *window, char **view);

void set_character(int x, int y, char c);

char get_char(int x, int y);