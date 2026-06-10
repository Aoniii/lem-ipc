#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stddef.h>
#include <stdlib.h>

unsigned char   *board_get(t_data *data) {
    return ((unsigned char *)data->shm_ptr + sizeof(t_shm_header));
}

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

void    board_set_empty(t_data *data, t_pos pos) {
    unsigned char   *board = board_get(data);
    board[pos.y * data->map_size + pos.x] = TILE_EMPTY;
}

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

bool    is_circled(t_data *data, t_pos *pos) {
    static const int    dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	static const int    dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	unsigned char       *board;
	unsigned char       tile;
	int                 counts[MAX_TEAM + 1];
	int                 nx;
	int                 ny;
	int                 i;
    int                 walls = 0;
    int                 enemies = 0;

	board = board_get(data);
	i = 0;
	while (i <= MAX_TEAM)
		counts[i++] = 0;

    i = 0;
	while (i < 8) {
		nx = pos->x + dx[i];
		ny = pos->y + dy[i];
		i++;

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
		    if (counts[tile] >= 2)
			    return (true);
        }

        if ((walls >= 6 && enemies >= 1) || walls + enemies >= 8)
            return (true);
	}
	return (false);
}

bool    is_game_over(t_data *data) {
    unsigned char   *board = board_get(data);
    unsigned char   tile;
    unsigned int    i = 0;
    int             team[MAX_TEAM + 1];
    int             count = 0;

    while (i <= MAX_TEAM)
		team[i++] = 0;

    i = 0;
    while (i < data->map_size * data->map_size) {
        tile = board[i];
        if (tile != TILE_EMPTY && tile != TILE_WALL) {
            if (team[tile] == 0) {
                team[tile] = 1;
                count++;
                if (count >= 2)
                    return (false);
            }
        }
        i++;
    }
    return (true);
}

void    walls_generator(t_data *data) {
    static const int    dx[4] = {0, 0, -1, 1};
	static const int    dy[4] = {-1, 1, 0, 0};

    unsigned char   *board = board_get(data);
    int             max = data->map_size * data->map_size;
    int             free_tile = max * (1 - WALL_RATIO);
    int             y = rand() % data->map_size;
    int             x = rand() % data->map_size;
    int             rdm;
    bool            new_direction;

    int i = 0;
    while (i < max)
        board[i++] = TILE_WALL;

    while (free_tile > 0) {
        if (board[y * data->map_size + x] == TILE_WALL) {
            board[y * data->map_size + x] = TILE_EMPTY;
            free_tile--;
        }
        new_direction = false;
        while (!new_direction) {
            rdm = rand() % 4;
            if (x + dx[rdm] < 0 || x + dx[rdm] >= (int)data->map_size || y + dy[rdm] < 0 || y + dy[rdm] >= (int)data->map_size)
                continue ;
            x += dx[rdm];
            y += dy[rdm];
            new_direction = true;
        }
    }
}
