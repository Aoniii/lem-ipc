#include "ipc.h"
#include "lem-ipc.h"
#include "display.h"
#include <sys/time.h>
#include <ncurses.h>
#include <locale.h>

// Standard terminal colors array used to dynamically generate team visual styles
static const int    g_colors[6] = {
    COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN
};

/**
 * @brief Pre-initializes color pairs for ncurses.
 * * Phase 1: Registers the first 6 teams with identical foreground/background 
 * colors (solid blocks).
 * * Phase 2: Generates up to 15 unique combinations of foreground/background 
 * mixes to support up to 21 distinct teams visual IDs.
 */
static void	init_team_colors(void) {
	int i;
	int a ;
	int b;

    i = 0;
	a = 0;
	b = 1;
    // Solid color blocks for the first 6 teams
	while (i < 6) {
		init_pair(i + 1, g_colors[i], g_colors[i]);
		i++;
	}

    // Bi-color combinations for subsequent teams (up to 21 or MAX_TEAM)
	while (i < 21 && i < MAX_TEAM) {
		init_pair(i + 1, g_colors[a], g_colors[b]);
		b++;
		if (b >= 6) {
			a++;
			b = a + 1;
		}
		i++;
	}

    // Special dark attribute used to create the blinking toggle effect
    init_pair(BLACK_ATTR, COLOR_BLACK, COLOR_BLACK);
}

/**
 * @brief Initializes the ncurses environment, locale settings, and window safety.
 * * Sets wide-character support (UTF-8), configures terminal behavior, initializes 
 * color pairs, and verifies if the terminal window size is large enough to render 
 * the grid layout, logs section, and status margins safely.
 */
int display_init(t_data *data) {
    int max_y;
    int max_x;
    int needed_y;
    int needed_x;

    setlocale(LC_ALL, "");  // Required for rendering UTF-8 box characters
    initscr();
    noecho();               // Do not echo user keystrokes to screen
    curs_set(0);            // Hide the hardware text cursor

    if (has_colors() == FALSE) {
        endwin();
        return (-1);
    }

    start_color();
    init_team_colors();

    // Screen dimension guard check
    getmaxyx(stdscr, max_y, max_x);
    needed_y = data->map_size + LOG_COUNT + MARGIN + 4;
    needed_x = MAX_MAP_SIZE * 2 + 2;
    if (max_y < needed_y || max_x < needed_x) {
        endwin();
        return (-1);
    }

    nodelay(stdscr, TRUE);  // Non-blocking keyboard input configurations
    return (0);
}

/**
 * @brief Draws a box outline using standard ncurses ACS (Alternate Character Set) symbols.
 */
static void draw_border(unsigned int height, unsigned int start_y) {
    unsigned int    width;
    unsigned int    i;

    width = MAX_MAP_SIZE * 2;
    // Draw corners
    mvaddch(start_y, 0, ACS_ULCORNER);
    mvaddch(start_y, width + 1, ACS_URCORNER);
    mvaddch(start_y + height + 1, 0, ACS_LLCORNER);
    mvaddch(start_y + height + 1, width + 1, ACS_LRCORNER);

    // Draw horizontal lines
    i = 1;
    while (i <= width) {
        mvaddch(start_y, i, ACS_HLINE);
        mvaddch(start_y + height + 1, i, ACS_HLINE);
        i++;
    }

    // Draw vertical lines
    i = 1;
    while (i <= height) {
        mvaddch(start_y + i, 0, ACS_VLINE);
        mvaddch(start_y + i, width + 1, ACS_VLINE);
        i++;
    }
}

/**
 * @brief Microsecond clock evaluator to drive the blinking animation.
 * * Uses system microtime to flip a boolean state back and forth based on the
 * defined `BLINK_TIME` constant frequency.
 */
static int blink_on(void) {
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_usec / BLINK_TIME) % 2 == 0);
}

/**
 * @brief Draws the board centered in the frame. Cells are 2 chars wide 
 * so they look square. The local player blinks.
 */
static void draw_map(t_data *data, unsigned char *snapshot, t_pos *pos) {
    unsigned int    y;
    unsigned int    x;
    unsigned char   tile;
    int             offset_x;
    int             offset_y;
    int             attr;

    offset_x = 1 + MAX_MAP_SIZE - data->map_size;
    offset_y = 1;
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
                attr = COLOR_PAIR(tile);
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

/**
 * @brief Displays the shared ring-buffer logs, oldest to newest.
 */
static void draw_logs(t_data *data, unsigned int start_y) {
    t_shm_header    *header;
    int             i;
    int             idx;

    header = (t_shm_header *)data->shm_ptr;
    i = 0;
    while (i < header->log_count) {
        idx = (header->log_head - header->log_count + i + LOG_COUNT) % LOG_COUNT;
        mvprintw(start_y + 1 + i, 2, "%s", header->logs[idx]);
        i++;
    }
}

/**
 * @brief Shows the player's team (or spectator status).
 */
static void draw_team_info(t_data *data, unsigned int start_y) {
    if (data->spectator) {
        mvprintw(start_y, 2, "You are spectator");
        return;
    }

    mvprintw(start_y, 2, "You are team: ");
    attron(COLOR_PAIR(data->team));
    addstr("\u2580\u2584"); // Print custom colorful square blocks
    attroff(COLOR_PAIR(data->team));
    printw(" #%u", data->team);
}

/**
 * @brief Displays the keyboard hotkey mappings legend for manual control modes.
 */
static void draw_controls(t_data *data, unsigned int start_y) {
	mvprintw(start_y, 2, "Q: quit");
    if (data->human)
	    mvprintw(start_y + 1, 2, "W: up   A: left   S: down   D: right");
}

/**
 * @brief Draws one full frame: board, info, controls, logs.
 */
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

/**
 * @brief Safely shuts down the ncurses context and restores original terminal settings.
 */
void    display_destroy(void) {
    endwin();
}
