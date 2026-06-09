#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stdlib.h>

static const int    g_dx[4] = {0, 0, -1, 1};
static const int    g_dy[4] = {-1, 1, 0, 0};

t_bfs_result    ai_bfs_multi(t_data *data, t_pos *sources, int count) {
	t_bfs_result    res;
	unsigned char   *board;
	int             *queue;
	int             *visited;
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
	board = board_get(data);
	size = data->map_size * data->map_size;
	
    queue = malloc(sizeof(int) * size);
	visited = malloc(sizeof(int) * size);
	if (!queue || !visited) {
        free(queue);
        free(visited);
        return (res);
    }

	i = 0;
	while (i < size)
		visited[i++] = 0;

	head = 0;
	tail = 0;
	i = 0;
	while (i < count) {
		cur = sources[i].y * data->map_size + sources[i].x;
		if (!visited[cur]) {
			visited[cur] = 1;
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
			if (visited[ni])
				continue ;
			tile = board[ni];
			if (tile == TILE_WALL)
				continue ;
			visited[ni] = 1;
			if (tile != TILE_EMPTY && tile != data->team) {
				res.found = true;
				res.target.x = nx;
				res.target.y = ny;
                free(queue);
                free(visited);
                return (res);
			}
			queue[tail++] = ni;
		}
	}
    free(queue);
    free(visited);
    return (res);
}

static t_pos    build_step(int *parent, t_data *data, t_pos src, int end)
{
	t_pos   step;

	int src_idx = src.y * data->map_size + src.x;
	int cur = end;
	while (parent[cur] != src_idx)
		cur = parent[cur];
	step.x = (cur % data->map_size) - src.x;
	step.y = (cur / data->map_size) - src.y;
	return (step);
}

static void	build_dir_order(t_pos src, t_pos dest, int *order) {
	int ddx;
	int ddy;
	int h;
	int v;

	ddx = dest.x - src.x;
	ddy = dest.y - src.y;
	h = (ddx > 0) ? 3 : 2;
	v = (ddy > 0) ? 1 : 0;
	if (abs(ddx) >= abs(ddy)) {
		order[0] = h;
		order[1] = v;
		order[2] = (ddy > 0) ? 0 : 1;
		order[3] = (ddx > 0) ? 2 : 3;
	} else {
		order[0] = v;
		order[1] = h;
		order[2] = (ddx > 0) ? 2 : 3;
		order[3] = (ddy > 0) ? 0 : 1;
	}
}

t_pos   ai_step_to(t_data *data, t_pos src, t_pos dest) {
	unsigned char   *board;
	int             *queue;
	int             *parent;
    int             order[4];
	int             size;
	int             head;
	int             tail;
	int             cur;
	int             nx;
	int             ny;
	int             ni;
	int             i;
	t_pos           step;

	step.x = 0;
	step.y = 0;
    if (abs(src.x - dest.x) + abs(src.y - dest.y) == 1)
		return (step);

	board = board_get(data);
	size = data->map_size * data->map_size;
	queue = malloc(sizeof(int) * size);
	parent = malloc(sizeof(int) * size);
	if (!queue || !parent) {
        free(queue);
        free(parent);
        return (step);
    }
	
    i = 0;
	while (i < size)
		parent[i++] = -1;
	head = 0;
	tail = 0;
	cur = src.y * data->map_size + src.x;
	parent[cur] = cur;
	queue[tail++] = cur;
    build_dir_order(src, dest, order);
    while (head < tail) {
		cur = queue[head++];
		if (cur != (src.y * (int)data->map_size + src.x)) {
			int cx = cur % data->map_size;
			int cy = cur / data->map_size;
			if (abs(cx - dest.x) + abs(cy - dest.y) == 1) {
				step = build_step(parent, data, src, cur);
                free(queue);
                free(parent);
                return (step);
			}
		}
		i = 0;
		while (i < 4) {
			nx = (cur % data->map_size) + g_dx[order[i]];
			ny = (cur / data->map_size) + g_dy[order[i]];
			i++;
			if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
				continue ;
			ni = ny * data->map_size + nx;
			if (parent[ni] != -1)
				continue ;
			if (board[ni] != TILE_EMPTY)
				continue ;
			parent[ni] = cur;
			queue[tail++] = ni;
		}
	}
    free(queue);
    free(parent);
    return (step);
}

