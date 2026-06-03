#include "lem-ipc.h"
#include "display.h"
#include <ncurses.h>
#include <locale.h>

static const int    g_colors[6] = {
    COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN
};

static void	init_team_colors(void) {
	int i = 0;
	int a = 0;
	int b = 1;

	while (i < 6) {
		init_pair(i + 1, g_colors[i], g_colors[i]);
		i++;
	}
	while (i < 21 && i < MAX_TEAM) {
		init_pair(i + 1, g_colors[a], g_colors[b]);
		b++;
		if (b >= 6) {
			a++;
			b = a + 1;
		}
		i++;
	}
}

int display_init(t_data *data) {
    int max_y;
    int max_x;
    int needed_y;
    int needed_x;

    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    if (has_colors() == FALSE) {
        endwin();
        return (-1);
    }
    start_color();
    init_team_colors();

    getmaxyx(stdscr, max_y, max_x);
    needed_y = data->map_size + 19;
    needed_x = MAX_MAP_SIZE * 2 + 6;
    if (max_y < needed_y || max_x < needed_x) {
        endwin();
        return (-1);
    }
    return (0);
}

static void draw_map_border(unsigned map_size) {
    unsigned int    width = MAX_MAP_SIZE * 2;
    unsigned int    i;

    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, width + 1, ACS_URCORNER);
    mvaddch(map_size + 1, 0, ACS_LLCORNER);
    mvaddch(map_size + 1, width + 1, ACS_LRCORNER);

    i = 1;
    while (i <= width) {
        mvaddch(0, i, ACS_HLINE);
        mvaddch(map_size + 1, i, ACS_HLINE);
        i++;
    }

    i = 1;
    while (i <= map_size) {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, width + 1, ACS_VLINE);
        i++;
    }
}

static void draw_map(t_data *data, unsigned char *snapshot, int offset_x, int offset_y) {
    unsigned int    y;
    unsigned int    x;
    unsigned char   tile;

    y = 0;
    while (y < data->map_size) {
        x = 0;
        while (x < data->map_size) {
            tile = snapshot[y * data->map_size + x];

            if (tile == TILE_WALL) {
                mvaddch(y + offset_y, (x + offset_x) * 2 - 1, '#');
                mvaddch(y + offset_y, (x + offset_x) * 2, '#');
            } else if (tile == TILE_EMPTY) {
                mvaddch(y + offset_y, (x + offset_x) * 2 - 1, ' ');
                mvaddch(y + offset_y, (x + offset_x) * 2, ' ');
            } else {
                attron(COLOR_PAIR(tile));
                mvaddstr(y + offset_y, (x + offset_x) * 2 - 1, "\u2580");
                mvaddstr(y + offset_y, (x + offset_x) * 2, "\u2584");
                attroff(COLOR_PAIR(tile));
            }
            x++;
        }
        y++;
    }
}

void    display_render(t_data *data, unsigned char *snapshot) {
    int offset_x = (MAX_MAP_SIZE - data->map_size) / 2 + 1;
    int offset_y = 1;

    clear();
    draw_map_border(data->map_size);
    draw_map(data, snapshot, offset_x, offset_y);
    refresh();
}

void    display_destroy(void) {
    endwin();
}
