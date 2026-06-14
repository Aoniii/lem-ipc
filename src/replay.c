#include "display.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"
#include "board.h"
#include "libft.h"
#include "print.h"
#include "libft.h"
#include "display.h"
#include "get_next_line_bonus.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ncurses.h>
#include <locale.h>

/**
 * @brief Initializes a new game replay file.
 * * This function retrieves the current working directory, ensures a "/replay" 
 * folder exists, and generates a timestamped filename (e.g., game-YYYYMMDD-HHMMSS.log).
 * The absolute path is saved into the shared memory for later use.
 */
int create_replay_file(t_data *data) {
    t_shm_header    *header;
    char            cwd[PATH_MAX];
    char            dir[PATH_MAX + 64];
    int             fd;
    time_t          now;
    struct tm       *t;

    header = (t_shm_header *)data->shm_ptr;

    // 1. Retrieve the absolute directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (-1);

    // 2. Create the replay folder (absolute path)
    snprintf(dir, sizeof(dir), "%s/replay", cwd);
    mkdir(dir, 0755);

    // 3. Generate the filename based on the date and time
    now = time(NULL);
    t = localtime(&now);
    snprintf(header->replay_path, sizeof(header->replay_path),
        "%s/replay/game-%04d%02d%02d-%02d%02d%02d.log",
        cwd, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    // 4. Create the file
    fd = open(header->replay_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return (-1);
    close(fd);
    return (0);
}

/**
 * @brief Writes the replay start header: map size, Initial board.
 * The board dump records the walls so the replay can reconstruct them.
 */
static void replay_write_header(t_data *data, int fd) {
    unsigned char   *board;
    char            line[32];
    char            c;
    unsigned int    i;
    int             len;

    board = board_get(data);

    // map size
    len = snprintf(line, sizeof(line), "MAP %u\n", data->map_size);
    write(fd, line, len);

    // Initial board : '1' = wall, '0' = empty; all on one line
    write(fd, "BOARD ", 6);
    i = 0;
    while (i < data->map_size * data->map_size) {
        c = (board[i] == TILE_WALL) ? '1' : '0';
        write(fd, &c, 1);
        i++;
    }
    write(fd, "\n", 1);
}

/**
 * @brief Opens the replay file in append mode.
 * If write_header is true, also writes the start header (map size, Initializes
 * board with walls, start timestamp). Return the fd, or -1 on failure.
 */
int replay_open(t_data *data, bool write_header) {
    t_shm_header    *header;
    int             fd;

    header = (t_shm_header *)data->shm_ptr;
    if (header->replay_path[0] == '\0')
        return (-1);

    fd = open(header->replay_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1)
        return (-1);

    if (write_header)
        replay_write_header(data, fd);
    return (fd);
}

/**
 * @brief Logs a event to the replay file.
 * Format: <ms> <id> <name> <team> <x> <y>
 * Must be called while holding the semaphore lock.
 */
void    replay_event(t_data *data, const char *name, t_pos pos) {
	t_shm_header    *header;
	char            line[64];
	long            elapsed;
	unsigned int    id;
	int             len;

	if (data->replay_fd == -1)
		return ;
	header = (t_shm_header *)data->shm_ptr;
	elapsed = now_ms() - header->start_ms;
	id = header->event_id++;
	len = snprintf(line, sizeof(line), "%ld %u %s %d %d %d\n", elapsed, id, name, data->team, pos.x, pos.y);
	write(data->replay_fd, line, len);
}

void    replay_join(t_data *data, t_pos pos) { replay_event(data, "JOIN", pos); }
void    replay_quit(t_data *data, t_pos pos) { replay_event(data, "QUIT", pos); }

/**
 * @brief Logs a MOVE event to the replay file.
 * Format: <ms> <id> MOVE <team> <x> <y> <dx> <dy>
 * Stores the position before moving and the direction.
 * Must be called while holding the semaphore lock.
 */
void    replay_move(t_data *data, t_pos old, int dx, int dy) {
	t_shm_header    *header;
	char            line[64];
	long            elapsed;
	unsigned int    id;
	int             len;

	if (data->replay_fd == -1)
		return ;
	header = (t_shm_header *)data->shm_ptr;
	elapsed = now_ms() - header->start_ms;
	id = header->event_id++;
	len = snprintf(line, sizeof(line), "%ld %u MOVE %d %d %d %d %d\n", elapsed, id, data->team, old.x, old.y, dx, dy);
	write(data->replay_fd, line, len);
}

static void free_split(char **split) {
	int i;

	if (!split)
		return ;

	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
}

/**
 * @brief Parses the replay header: "MAP <size>" and "BOARD <0/1...>".
 * Validates the size and that the board string only contains 0/1, then
 * fills map_size and initial_board.
 */
static int  parse_header(t_replay *replay, int fd) {
	char    *line;
	char    **parts;
	int	    i;

	// --- line MAP ---
	line = get_next_line(fd);
	if (!line)
		return (-1);

	parts = ft_split(line, ' ');
	free(line);
	if (!parts || !parts[0] || !parts[1] || ft_strncmp(parts[0], "MAP", ft_strlen(parts[0])) != 0) {
        free_split(parts);
        return (-1);
    }

	replay->map_size = ft_atoi(parts[1]);
	free_split(parts);
	if (replay->map_size < MIN_MAP_SIZE || replay->map_size > MAX_MAP_SIZE)
		return (-1);

	// --- line BOARD ---
	line = get_next_line(fd);
	if (!line)
		return (-1);

    i = 0;
    while (line[i] != '\n')
        i++;
    line[i] = 0;

	parts = ft_split(line, ' ');
	free(line);
	if (!parts || !parts[0] || !parts[1] || ft_strncmp(parts[0], "BOARD", ft_strlen(parts[0])) != 0) {
        free_split(parts);
        return (-1);
    }

    // check that the string is exactly map_size * map_size in length
	if (ft_strlen(parts[1]) != replay->map_size * replay->map_size) {
        free_split(parts);
        return (-1);
    }

	replay->initial_board = malloc(replay->map_size * replay->map_size);
	if (!replay->initial_board) {
        free_split(parts);
        return (-1);
    }

    // copy board
	i = 0;
	while (parts[1][i]) {
		if (parts[1][i] == '0') {
			replay->initial_board[i] = TILE_EMPTY;
        } else if (parts[1][i] == '1') {
			replay->initial_board[i] = TILE_WALL;
        } else {
            free_split(parts);
            return (-1);
        }
		i++;
	}

	free_split(parts);
	return (0);
}

/**
 * @brief Appends an event node to the end of the replay's linked list.
 */
static void append_event(t_replay *replay, t_event *ev) {
	t_event *tail;

	ev->next = NULL;
	if (!replay->head) {
		ev->prev = NULL;
		replay->head = ev;
		return ;
	}
	tail = replay->head;
	while (tail->next)
		tail = tail->next;
	ev->prev = tail;
	tail->next = ev;
}

// returns the event type, or -1 if the name is unknown
static int  event_type_from_str(const char *s) {
	if (ft_strncmp(s, "JOIN", 5) == 0)
		return (EV_JOIN);
	if (ft_strncmp(s, "QUIT", 5) == 0)
		return (EV_QUIT);
	if (ft_strncmp(s, "MOVE", 5) == 0)
		return (EV_MOVE);
	return (-1);
}

/**
 * @brief Parses one event line and appends it to the list.
 * Format: <ms> <id> <TYPE> <team> <x> <y> [<dx> <dy>]
 * Checks ms is non-decreasing, id increments by 1, the type is known,
 * coordinates are in bounds, and (for MOVE) the direction is one step.
 */
static int  parse_event(t_replay *replay, char *line, long *last_ms, unsigned int *next_id) {
	char    **p;
	t_event *ev;
	int     type;

	p = ft_split(line, ' ');
	if (!p || !p[0] || !p[1] || !p[2] || !p[3] || !p[4] || !p[5]) {
        free_split(p);
        return (-1);
    }

	type = event_type_from_str(p[2]);
	if (type == -1) {
        free_split(p);
        return (-1);
    }

	// MOVE needs two extra fields (dx dy)
	if (type == EV_MOVE && (!p[6] || !p[7])) {
        free_split(p);
        return (-1);
    }

	ev = malloc(sizeof(t_event));
	if (!ev) {
        free_split(p);
        return (-1);
    }

	ft_memset(ev, 0, sizeof(t_event));
	ev->ms = ft_atoi(p[0]);
	ev->id = (unsigned int)ft_atoi(p[1]);
	ev->event = type;
	ev->team = (unsigned char)ft_atoi(p[3]);
	ev->x = ft_atoi(p[4]);
	ev->y = ft_atoi(p[5]);
	if (type == EV_MOVE) {
		ev->dx = ft_atoi(p[6]);
		ev->dy = ft_atoi(p[7]);
	}
	free_split(p);

	// --- validations ---
    // ms non-decreasing
	if (ev->ms < *last_ms) {
        free(ev);
        return (-1);
    }

    // id increments by 1
	if (ev->id != *next_id) {
        free(ev);
        return (-1);
    }

    // in bounds
	if (ev->x < 0 || ev->x >= (int)replay->map_size || ev->y < 0 || ev->y >= (int)replay->map_size) {
        free(ev);
        return (-1);
    }

    // one orthogonal step
	if (type == EV_MOVE && (abs(ev->dx) + abs(ev->dy) != 1)) {
        free(ev);
        return (-1);
    }

	*last_ms = ev->ms;
	*next_id = ev->id + 1;
	append_event(replay, ev);
	return (0);
}

/**
 * @brief Reads and parses the whole replay file into the replay struct.
 * Parses the header, then each event line, validating format and order.
 */
static int  replay_parse(t_replay *replay, int fd) {
	char            *line;
	long            last_ms;
	unsigned int    next_id;

	if (parse_header(replay, fd) == -1)
		return (-1);

	last_ms = 0;
	next_id = 0;
	line = get_next_line(fd);
	while (line) {
		if (parse_event(replay, line, &last_ms, &next_id) == -1) {
			free(line);
			return (-1);
		}
		free(line);
		line = get_next_line(fd);
	}

	// init the working board as a copy of the initial one
	replay->board = malloc(replay->map_size * replay->map_size);
	if (!replay->board)
		return (-1);

	ft_memcpy(replay->board, replay->initial_board, replay->map_size * replay->map_size);
	replay->current = replay->head;
	return (0);
}

/**
 * @brief Applies an event to the current board (move forward).
 * JOIN places the team, MOVE shifts it, QUIT clears it.
 */
static void apply_event(t_replay *replay, t_event *ev) {
	unsigned char   *board = replay->board;
	unsigned int    w = replay->map_size;

	if (ev->event == EV_JOIN) {
		board[ev->y * w + ev->x] = ev->team;
    } else if (ev->event == EV_QUIT) {
		board[ev->y * w + ev->x] = TILE_EMPTY;
    } else if (ev->event == EV_MOVE) {
		board[ev->y * w + ev->x] = TILE_EMPTY;
		board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] = ev->team;
	}
}

/**
 * @brief Reverts an event on the current board (move backward).
 * Undoes exactly what apply_event did.
 */
static void undo_event(t_replay *replay, t_event *ev) {
	unsigned char   *board = replay->board;
	unsigned int    w = replay->map_size;

	if (ev->event == EV_JOIN) {
		board[ev->y * w + ev->x] = TILE_EMPTY;
    } else if (ev->event == EV_QUIT) {
		board[ev->y * w + ev->x] = ev->team;
    } else if (ev->event == EV_MOVE) {
		board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] = TILE_EMPTY;
		board[ev->y * w + ev->x] = ev->team;
	}
}

static t_event  *replay_last(t_replay *replay) {
	t_event *ev = replay->head;

	if (!ev)
		return (NULL);
	while (ev->next)
		ev = ev->next;
    return (ev);
}

// skips to the next event. Returns false if there are no more events to process.
static bool step_forward(t_replay *replay) {
	if (!replay->current)
		return (false);
	apply_event(replay, replay->current);
	replay->ms_saved = replay->current->ms;
	replay->current = replay->current->next;
	return (true);
}

// moves back one event. Returns false if the current position is already at the beginning.
static bool step_backward(t_replay *replay) {
	t_event *ev;

    // the event to cancel is the one immediately before the current one
	if (replay->current)
		ev = replay->current->prev;
	else
		ev = replay_last(replay);   // if current == NULL (end), take the last one

	if (!ev)
		return (false);             // already from the start

	undo_event(replay, ev);
	replay->current = ev;
	replay->ms_saved = ev->ms;
	return (true);
}

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

static int  replay_play(t_replay *replay) {
	int     ch;
	long    last_tick;

	if (display_init_replay(replay) == -1)
		return (-1);

	replay->playing = false;
	last_tick = now_ms();

	while (1) {
		ch = getch();
		if (ch == 'q' || ch == 'Q') //quit
			break ;

		if (ch == ' ') {    // play/pause
            replay->playing = !replay->playing;
        } else if (ch == KEY_RIGHT && !replay->playing) {
            step_forward(replay);
        } else if (ch == KEY_LEFT && !replay->playing) {
            step_backward(replay);
        }
		
        if (replay->playing) {
			long now = now_ms();
			replay->ms_saved += (now - last_tick);
			last_tick = now;

			// apply all events where ms <= ms_saved
			while (replay->current && replay->current->ms <= replay->ms_saved)
				step_forward(replay);

			if (!replay->current)
                replay->playing = false;
		} else {
            last_tick = now_ms();    // paused, no accumulation
		}

		display_render_replay(replay);
		usleep(1000000 / FPS);
	}

	display_destroy();
	return (0);
}

/**
 * @brief Frees everything allocated in the replay struct.
 * Walks the event list freeing each node, then the two boards.
 */
static void replay_free(t_replay *replay) {
	t_event *cur;
	t_event *next;

	cur = replay->head;
	while (cur) {
		next = cur->next;
		free(cur);
		cur = next;
	}

	replay->head = NULL;
	replay->current = NULL;
	free(replay->initial_board);
	free(replay->board);
	replay->initial_board = NULL;
	replay->board = NULL;
}

int replay_start(char *filename) {
    t_replay    replay;
    int         fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        ft_printf("lemipc: error: failed to open the replay file\n");
        return (-1);
    }

    ft_memset(&replay, 0, sizeof(t_replay));
    if (replay_parse(&replay, fd) == -1) {
        close(fd);
        replay_free(&replay);
        ft_printf("lemipc: error: failed to parse the replay file\n");
        return (-1);
    }

    close(fd);
    if (replay_play(&replay) == -1) {
        replay_free(&replay);
		ft_printf("lemipc: error: unplayable event in replay\n");
        return (-1);
    }

    replay_free(&replay);
    return (0);
}
