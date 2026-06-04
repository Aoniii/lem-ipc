#include "ipc.h"
#include "lem-ipc.h"
#include "display.h"
#include <sys/time.h>
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

    init_pair(BLACK_ATTR, COLOR_BLACK, COLOR_BLACK);
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

    nodelay(stdscr, TRUE);
    return (0);
}

static void draw_border(unsigned int height, unsigned int start_y) {
    unsigned int    width = MAX_MAP_SIZE * 2;
    unsigned int    i;

    mvaddch(start_y, 0, ACS_ULCORNER);
    mvaddch(start_y, width + 1, ACS_URCORNER);
    mvaddch(start_y + height + 1, 0, ACS_LLCORNER);
    mvaddch(start_y + height + 1, width + 1, ACS_LRCORNER);

    i = 1;
    while (i <= width) {
        mvaddch(start_y, i, ACS_HLINE);
        mvaddch(start_y + height + 1, i, ACS_HLINE);
        i++;
    }

    i = 1;
    while (i <= height) {
        mvaddch(start_y + i, 0, ACS_VLINE);
        mvaddch(start_y + i, width + 1, ACS_VLINE);
        i++;
    }
}

static int blink_on(void) {
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_usec / BLINK_TIME) % 2 == 0);
}

static void draw_map(t_data *data, unsigned char *snapshot, t_pos *pos) {
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
                int attr = COLOR_PAIR(tile);
                if (pos && (int)x == pos->x && (int)y == pos->y && !blink_on())
                    attr = COLOR_PAIR(BLACK_ATTR);

                attron(attr);
                mvaddstr(y + offset_y, x * 2 + offset_x, "\u2580");
                mvaddstr(y + offset_y, x * 2 + offset_x + 1, "\u2584");
                attroff(attr);
            }
            x++;
        }
        y++;
    }
}

static void draw_logs(t_data *data, unsigned int start_y) {
    t_shm_header    *header = (t_shm_header *)data->shm_ptr;
    int             i = 0;
    int             idx;

    while (i < header->log_count) {
        idx = (header->log_head - header->log_count + i + LOG_COUNT) % LOG_COUNT;
        mvprintw(start_y + 1 + i, 2, "%s", header->logs[idx]);
        i++;
    }
}

static void draw_team_info(t_data *data, unsigned int start_y) {
    if (data->spectator) {
        mvprintw(start_y, 2, "You are spectator");
        return;
    }

    mvprintw(start_y, 2, "You are team: ");
    attron(COLOR_PAIR(data->team));
    addstr("\u2580\u2584");
    attroff(COLOR_PAIR(data->team));
    printw(" #%u", data->team);
}

static void draw_controls(t_data *data, unsigned int start_y) {
	mvprintw(start_y, 2, "Q: quit");
    if (data->human)
	    mvprintw(start_y + 1, 2, "W: up   A: left   S: down   D: right");
}

void    display_render(t_data *data, unsigned char *snapshot, t_pos *pos) {
    clear();
    draw_border(data->map_size, 0);
    draw_map(data, snapshot, pos);
    draw_team_info(data, data->map_size + 2);
    draw_controls(data, data->map_size + 4);
    draw_border(5, data->map_size + MARGIN);
    draw_logs(data, data->map_size + MARGIN);
    refresh();
}

void    display_destroy(void) {
    endwin();
}
