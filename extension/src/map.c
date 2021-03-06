#include "headers/game.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define MAX_PELLETS 189
#define UI_HEIGHT 2
#define REPLACE_BLOCK(block) (block == '#' ? ' ' | A_REVERSE : block)

void print_char_colour(Game_Constructs_t object, int x, int y, unsigned char c, int frightened_mode)
{
    switch (frightened_mode)
    {
    case 1:
        init_pair(object, COLOR_BLUE, -1);
        break;
    case 0:
        switch (object)
        {
        case INKY_E:
            init_pair(object, COLOR_CYAN, -1);
            break;
        case BLINKY_E:
            init_pair(object, COLOR_RED, -1);
            break;
        case PINKY_E:
            init_pair(object, COLOR_MAGENTA, -1);
            break;
        case CLYDE_E:
            init_pair(object, COLOR_YELLOW, -1);
            break;
        case PACMAN_E:
            init_pair(object, COLOR_YELLOW, -1);
            break;
        case WALL_E:
            init_pair(object, COLOR_BLUE, -1);
            break;
        default:
            init_pair(object, -1, -1);
        }
    }
    attron(COLOR_PAIR(object));
    mvaddch(y + UI_HEIGHT, x, REPLACE_BLOCK(c == 'b' ? ' ' : c));
    attroff(COLOR_PAIR(object));
}

void print_view(WINDOW *window, game_t *game)
{
    clear();

    mvprintw(0, 1, "1UP\t\t HIGH SCORE");
    mvprintw(1, 1, "  %d\t\t%9d", game->points, game->high_score);

    //print ghosts
    for (int i = 0; i < game->num_ghosts; i++)
    {
        print_char_colour(GHOSTS[i]->ghost_name, GHOSTS[i]->x, GHOSTS[i]->y, GHOST_REPRESENTATION, GHOSTS[i]->mode == FRIGHTENED);
    }

    //print remaining
    int i = 0;
    for (; game->map->view[i]; i++)
    {
        for (int j = 0; MAP->view[i][j]; j++)
        {
            if (MAP->view[i][j] != GHOST_REPRESENTATION)
            {
                print_char_colour(MAP->view[i][j], j, i, MAP->view[i][j], 0);
            }
        }
    }

    for (int j = 0; j < game->lives - 1; j++)
    {
        print_char_colour(PACMAN_REPRESENTATION, j * 2, i, PACMAN_REPRESENTATION, 0);
    }

    wrefresh(window);
    usleep(50000);
}

void set_char(int x, int y, char c, map_t *map)
{
    map->view[y][x] = c;
}

char get_char(int x, int y, map_t *map)
{
    return map->view[y][x];
}

static int get_max_x(char **view)
{
    return strlen(view[0]);
}

static int get_max_y(char **view)
{
    int y = 0;
    for (; view[y]; y++)
        ;
    return y;
}

void create_map(game_t *game)
{
    MAP = (map_t *)malloc(sizeof(map_t));
    assert(MAP);
}

void free_map(game_t *game)
{
    for (int i = 0; MAP->view[i]; i++)
    {
        free(MAP->view[i]);
    }
    free(MAP->view);
    free(MAP);
}

void init_map(game_t *game)
{
    char *view_test[24] = {
        "#######################################b",
        "#b. . . . . . . .b###b. . . . . . . .b#b",
        "#bOb###b.b#####b.b###b.b#####b.b###bOb#b",
        "#b. . . . . . . . . . . . . . . . . .b#b",
        "#b.b###b.b#b.b###########b.b#b.b###b.b#b",
        "#b. . . .b#b. . .b###b. . .b#b. . . .b#b",
        "#######b.b#####b.b###b.b#####b.b#######b",
        "     b#b.b#b. . . . . . . .b#b.b#b      ",
        "     b#b.b#b.b###-----###b.b#b.b#b      ",
        "#######b.b#b.b#b       b#b.b#b.b########",
        "        . . .b#b       b#b. . .         ",
        "#######b.b#b.b#b       b#b.b#b.b########",
        "     b#b.b#b.b###########b.b#b.b#b      ",
        "     b#b.b#b. . . . . . . .b#b.b#b      ",
        "#######b.b#b.b###########b.b#b.b#######b",
        "#b. . . . . . . .b###b. . . . . . . .b#b",
        "#bOb###b.b#####b.b###b.b#####b.b###bOb#b",
        "#b. .b#b. . . . . . . . . . . .b#b. .b#b",
        "###b.b#b.b#b.b###########b.b#b.b#b.b###b",
        "#b. . . .b#b. . .b###b. . .b#b. . . .b#b",
        "#b.b###########b.b###b.b###########b.b#b",
        "#b. . . . . . . . . . . . . . . . . .b#b",
        "#######################################b",
        NULL};

    MAP->view = calloc(24, sizeof(char *));
    assert(MAP->view);
    for (int i = 0; i < 23; i++)
    {
        MAP->view[i] = strdup(view_test[i]);
    }
    MAP->view[24] = NULL;

    MAP->max_x = get_max_x(MAP->view);
    MAP->max_y = get_max_y(MAP->view);

    MAP->pacman_start_offset = 6;

    MAP->pellet_num = MAX_PELLETS;
}
