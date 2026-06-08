// src/ai.c
#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stdlib.h>

static const int    g_dx[4] = {0, 0, -1, 1};
static const int    g_dy[4] = {-1, 1, 0, 0};

static void compute_first_step(int *parent, int *root, t_data *data, t_bfs_result *res, t_pos *sources) {
	int cur = res->target.y * data->map_size + res->target.x;
	int src_idx = root[cur];
	while (parent[cur] != cur && parent[parent[cur]] != parent[cur])
		cur = parent[cur];
	res->dx = (cur % data->map_size) - sources[src_idx].x;
	res->dy = (cur / data->map_size) - sources[src_idx].y;
}

t_bfs_result    ai_bfs_multi(t_data *data, t_pos *sources, int count) {
	t_bfs_result    res;
	unsigned char   *board;
	int             *queue;
	int             *parent;
	int             *root;
	int             size;
	int             head;
	int             tail;
	int             cur;
	int             nx;
	int             ny;
	int             ni;
	int             i;
	unsigned char   tile;

	res.found = false;
	res.dx = 0;
	res.dy = 0;
	board = board_get(data);
	size = data->map_size * data->map_size;

	queue = malloc(sizeof(int) * size);
	parent = malloc(sizeof(int) * size);
	root = malloc(sizeof(int) * size);
	if (!queue || !parent || !root) {
		free(queue);
		free(parent);
		free(root);
		return (res);
	}
	
    i = 0;
	while (i < size) {
		parent[i] = -1;
		root[i] = -1;
		i++;
	}

	head = 0;
	tail = 0;
	i = 0;
	while (i < count) {
		cur = sources[i].y * data->map_size + sources[i].x;
		if (parent[cur] == -1) {
			parent[cur] = cur;
			root[cur] = i;
			queue[tail++] = cur;
		}
		i++;
	}

	while (head < tail) {
		cur = queue[head++];
		i = 0;
		while (i < 4) {
			nx = (cur % data->map_size) + g_dx[i];
			ny = (cur / data->map_size) + g_dy[i];
			i++;
			if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
				continue ;
			ni = ny * data->map_size + nx;
			if (parent[ni] != -1)
				continue ;
			tile = board[ni];
			if (tile == TILE_WALL)
				continue ;
			parent[ni] = cur;
			root[ni] = root[cur];
			if (tile != TILE_EMPTY && tile != data->team) {
				res.found = true;
				res.target.x = nx;
				res.target.y = ny;
				compute_first_step(parent, root, data, &res, sources);
				free(queue);
				free(parent);
				free(root);
				return (res);
			}
			queue[tail++] = ni;
		}
	}
	free(queue);
	free(parent);
	free(root);
	return (res);
}
