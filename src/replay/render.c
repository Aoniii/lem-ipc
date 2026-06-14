#include "display.h"
#include "replay.h"
#include "lem-ipc.h"
#include <locale.h>
#include <ncurses.h>

/**
 * @brief Initializes ncurses for replay playback.
 * Same setup as the game display, but enables arrow keys for navigation.
 */
int display_init_replay(t_replay *replay) {
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
	needed_y = replay->map_size + 6;
	needed_x = MAX_MAP_SIZE * 2 + 2;

	if (max_y < needed_y || max_x < needed_x) {
        endwin();
        return (-1);
    }

	nodelay(stdscr, TRUE);   // non-blocking input
	keypad(stdscr, TRUE);    // capture arrow keys
	return (0);
}

/**
 * @brief Draws the replay board, centered in the frame.
 * Same rendering as the game (walls, empty, team half-blocks) but reads
 * from the replay's reconstructed board.
 */
static void draw_replay_map(t_replay *replay) {
	int             offset_x = 1 + MAX_MAP_SIZE - replay->map_size;
	int             offset_y = 1;
	unsigned int    y;
	unsigned int    x;
	unsigned char   tile;

	y = 0;
	while (y < replay->map_size) {
		x = 0;
		while (x < replay->map_size) {
			tile = replay->board[y * replay->map_size + x];
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

/**
 * @brief Renders one full replay frame: board, border, and status bar.
 */
void    display_render_replay(t_replay *replay) {
	clear();
	draw_border(replay->map_size, 0);
	draw_replay_map(replay);
	// status bar below the board
	mvprintw(replay->map_size + 2, 2, "%s   time: %ld ms", replay->playing ? "[PLAYING]" : "[PAUSED] ", replay->ms_saved);
	mvprintw(replay->map_size + 3, 2, "SPACE: play/pause   <- -> : step   Q: quit");
	refresh();
}
