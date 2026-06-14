#include "replay.h"
#include "lem-ipc.h"
#include "get_next_line_bonus.h"
#include <stdlib.h>

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
    while (line[i] && line[i] != '\n')
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

    // check team
    if (ev->team >= MAX_TEAM) {
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
int  replay_parse(t_replay *replay, int fd) {
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
