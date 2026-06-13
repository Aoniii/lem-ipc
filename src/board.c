#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stddef.h>
#include <stdlib.h>

/**
 * @brief Retrieves the pointer to the actual game board in shared memory.
 * * Skipping the shared memory header (`t_shm_header`), this function calculates
 * the precise memory address where the game map grid starts.
 */
unsigned char   *board_get(t_data *data) {
    return ((unsigned char *)data->shm_ptr + sizeof(t_shm_header));
}

/**
 * @brief Counts the total occurrences of a specific tile value on the board.
 */
unsigned int    board_count(t_data *data, unsigned char value) {
    unsigned char   *board = board_get(data);
    unsigned int    size = data->map_size * data->map_size;
    unsigned int    i = 0;
    unsigned int    ret = 0;

    while (i < size) {
        if (board[i] == value)
            ret++;
        i++;
    }

    return (ret);
}

/**
 * @brief Clears a specific tile on the board, making it empty.
 */
void    board_set_empty(t_data *data, t_pos pos) {
    unsigned char   *board;

    board = board_get(data);
    board[pos.y * data->map_size + pos.x] = TILE_EMPTY;
}

/**
 * @brief Locates and stores all active positions of teammates.
 * Scans the board and gathers coordinates of all players belonging to the same team.
 */
int team_positions(t_data *data, t_pos *out, int max) {
	unsigned char   *board;
	unsigned int    x;
	unsigned int    y;
	int             n;

	board = board_get(data);
	n = 0;
	y = 0;
	while (y < data->map_size && n < max) {
		x = 0;
		while (x < data->map_size && n < max) {
			if (board[y * data->map_size + x] == data->team) {
				out[n].x = x;
				out[n].y = y;
				n++;
			}
			x++;
		}
		y++;
	}
	return (n);
}

/**
 * @brief Checks if a player at a given position is eliminated by encirclement.
 * * Implements the core game logic: an agent dies if surrounded by 2 or more 
 * players of the same enemy team, or if trapped completely by walls and enemies.
 * Evaluates all 8 neighboring directions (orthogonal and diagonal).
 */
bool    is_circled(t_data *data, t_pos *pos) {
    // 8-directional offsets (including diagonals)
    static const int    dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	static const int    dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

	unsigned char   *board;
	unsigned char   tile;
	int             counts[MAX_TEAM + 1];
	int             nx;
	int             ny;
	int             i;
    int             walls = 0;
    int             enemies = 0;

	board = board_get(data);

    // Reset enemy team tracking array
	i = 0;
	while (i <= MAX_TEAM)
		counts[i++] = 0;

    i = 0;
	while (i < 8) {
		nx = pos->x + dx[i];
		ny = pos->y + dy[i];
		i++;

        // Boundary safety check
		if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
			continue ;

		tile = board[ny * data->map_size + nx];
		if (tile == TILE_EMPTY || tile == data->team)
			continue ;

        if (tile == TILE_WALL) {
            walls++;
        } else {
		    counts[tile]++;
            enemies++;
            // Core Rule: At least 2 players from the SAME enemy team are adjacent
		    if (counts[tile] >= 2)
			    return (true);
        }

        /* * BONUS WALLS CONDITION (Non-mandatory):
         * If wall generation is enabled, check for map-trapping.
         * Prevents stalemates where a player hugs a corner/wall 
         * to become unkillable by normal rules.
         */
        if (data->walls && ((walls >= 6 && enemies >= 1) || walls + enemies >= 8))
            return (true);
	}
	return (false);
}

/**
 * @brief Checks whether the match has ended.
 * Iterates through the board to count how many distinct teams still have active agents.
 */
bool    is_game_over(t_data *data) {
    unsigned char   *board = board_get(data);
    unsigned char   tile;
    unsigned int    i = 0;
    int             team[MAX_TEAM + 1];
    int             count = 0;

    // Reset team presence tracker
    while (i <= MAX_TEAM)
		team[i++] = 0;

    i = 0;
    while (i < data->map_size * data->map_size) {
        tile = board[i];
        if (tile != TILE_EMPTY && tile != TILE_WALL) {
            if (team[tile] == 0) {
                team[tile] = 1;
                count++;
                // If at least two different teams are alive, the game continues
                if (count >= 2)
                    return (false);
            }
        }
        i++;
    }
    return (true);
}

/**
 * @brief Generates procedural map walls using a Random Walk algorithm.
 * * Fills the entire board with walls, then digs paths starting from a random 
 * location until the requested empty space ratio (`WALL_RATIO`) is reached.
 * This guarantees that all empty paths generated are contiguous (no isolated pockets).
 */
void    walls_generator(t_data *data) {
    // Directional offset arrays (dx/dy) for: Up, Down, Left, Right
    static const int    dx[4] = {0, 0, -1, 1};
	static const int    dy[4] = {-1, 1, 0, 0};

    unsigned char   *board;
    int             max;
    int             free_tile;
    int             y;
    int             x;
    int             rdm;
    int             i;
    bool            new_direction;

    board = board_get(data);
    max = data->map_size * data->map_size;
    free_tile = max * (1 - WALL_RATIO); // Number of empty tiles to carve out
    y = rand() % data->map_size;
    x = rand() % data->map_size;

    // 1. Initialize the entire map as a solid block of walls
    i = 0;
    while (i < max)
        board[i++] = TILE_WALL;

    // 2. Carve paths using a random walker
    while (free_tile > 0) {
        if (board[y * data->map_size + x] == TILE_WALL) {
            board[y * data->map_size + x] = TILE_EMPTY;
            free_tile--;
        }
        new_direction = false;
        while (!new_direction) {
            rdm = rand() % 4;

            // Boundary safety check for the next random step
            if (x + dx[rdm] < 0 || x + dx[rdm] >= (int)data->map_size || y + dy[rdm] < 0 || y + dy[rdm] >= (int)data->map_size)
                continue ;

            x += dx[rdm];
            y += dy[rdm];
            new_direction = true;
        }
    }
}
