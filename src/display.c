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
    needed_y = data->map_size + LOG_COUNT + MARGIN + 4;
    needed_x = MAX_MAP_SIZE * 2 + 2;
    if (max_y < needed_y || max_x < needed_x) {
        endwin();
        return (-1);
    }
    return (0);
}

static void draw_border(unsigned int start_height, unsigned int height) {
    unsigned int    width = MAX_MAP_SIZE * 2;
    unsigned int    i;

    mvaddch(start_height, 0, ACS_ULCORNER);
    mvaddch(start_height, width + 1, ACS_URCORNER);
    mvaddch(start_height + height + 1, 0, ACS_LLCORNER);
    mvaddch(start_height + height + 1, width + 1, ACS_LRCORNER);

    i = 1;
    while (i <= width) {
        mvaddch(start_height, i, ACS_HLINE);
        mvaddch(start_height + height + 1, i, ACS_HLINE);
        i++;
    }

    i = 1;
    while (i <= height) {
        mvaddch(start_height + i, 0, ACS_VLINE);
        mvaddch(start_height + i, width + 1, ACS_VLINE);
        i++;
    }
}

static void draw_map(t_data *data, unsigned char *snapshot) {
    int offset_x = 1 + MAX_MAP_SIZE - data->map_size;
    int offset_y = 1;

    unsigned int    y;
    unsigned int    x;
    unsigned char   tile;

    y = 0;
    while (y < data->map_size) {
        x = 0;
        while (x < data->map_size) {
            tile = snapshot[y * data->map_size + x];

            if (tile == TILE_WALL) {
                mvaddch(y + offset_y, x * 2 + offset_x, '#');
                mvaddch(y + offset_y, x * 2 + offset_x + 1, '#');
            } else if (tile == TILE_EMPTY) {
                mvaddch(y + offset_y, x * 2 + offset_x, '.');
                mvaddch(y + offset_y, x * 2 + offset_x + 1, '.');
            } else {
                attron(COLOR_PAIR(tile));
                mvaddstr(y + offset_y, x * 2 + offset_x, "\u2580");
                mvaddstr(y + offset_y, x * 2 + offset_x + 1, "\u2584");
                attroff(COLOR_PAIR(tile));
            }
            x++;
        }
        y++;
    }
}

static void draw_logs(t_logs *logs, unsigned int start_y) {
    int i;
    int idx;

    i = 0;
    while (i < logs->count) {
        idx = (logs->head - logs->count + i + LOG_COUNT) % LOG_COUNT;
        mvprintw(start_y + 1 + i, 2, "%s", logs->lines[idx]);
        i++;
    }
}

void    display_render(t_data *data, unsigned char *snapshot, t_logs *logs) {
    clear();
    draw_border(0, data->map_size);
    draw_map(data, snapshot);
    draw_border(data->map_size + MARGIN, 5);
    draw_logs(logs, data->map_size + MARGIN);
    refresh();
}

void    display_destroy(void) {
    endwin();
}
