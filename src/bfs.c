#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stdlib.h>

// Global directional arrays for neighbor exploration: Up, Down, Left, Right
static const int    g_dx[4] = {0, 0, -1, 1};
static const int    g_dy[4] = {-1, 1, 0, 0};

/**
 * @brief Multi-source Breadth-First Search (BFS) to find the nearest enemy.
 * * Explores the map starting from multiple source positions simultaneously (e.g., all teammates).
 * It stops and returns the coordinates of the first valid enemy target encountered.
 */
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

    // 1. Allocate memory for queue and visited tracking array
    queue = malloc(sizeof(int) * size);
	visited = malloc(sizeof(int) * size);
	if (!queue || !visited) {
        free(queue);
        free(visited);
        return (res);
    }

    // 2. Initialize visited array to 0
	i = 0;
	while (i < size)
		visited[i++] = 0;

	head = 0;
	tail = 0;

    // 3. Push all starting sources into the queue to perform multi-source exploration
	i = 0;
	while (i < count) {
		cur = sources[i].y * data->map_size + sources[i].x;
		if (!visited[cur]) {
			visited[cur] = 1;
			queue[tail++] = cur;
		}
		i++;
	}

    // 4. Standard BFS loop
	while (head < tail) {
		cur = queue[head++];
		i = 0;
        // Explore all 4 adjacent directions
		while (i < 4) {
			nx = (cur % data->map_size) + g_dx[i];
			ny = (cur / data->map_size) + g_dy[i];
			i++;

            // Boundary check
			if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
				continue ;

			ni = ny * data->map_size + nx;
			if (visited[ni])
				continue ;

			tile = board[ni];
			if (tile == TILE_WALL)
				continue ;

			visited[ni] = 1;

            // Target found: Tile is not empty, and does not belong to our team (Enemy)
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

/**
 * @brief Backtracks from the destination tile to find the very first step to take.
 * * Follows the parent links backwards from the end tile until reaching the tile 
 * immediately adjacent to the source position.
 */
static t_pos    build_step(int *parent, t_data *data, t_pos src, int end)
{
	t_pos   step;
	int     src_idx = src.y * data->map_size + src.x;
	int     cur = end;

    // Backtrack until the current tile's parent is the starting position
	while (parent[cur] != src_idx)
		cur = parent[cur];

    // Calculate the movement vector from src to the first node of the path
	step.x = (cur % data->map_size) - src.x;
	step.y = (cur / data->map_size) - src.y;
	return (step);
}

/**
 * @brief Determines the optimal ordering of directions to optimize BFS exploration.
 * * Prioritizes movement indices based on the general direction of the destination
 * to make the BFS find the path faster and handle dead-ends gracefully.
 */
static void build_dir_order(t_pos src, t_pos dest, int *order) {
	int ddx;
	int ddy;
	int h;
	int v;

	ddx = dest.x - src.x;
	ddy = dest.y - src.y;

    // Choose specific direction indices based on sign: Up(0), Down(1), Left(2), Right(3)
	h = (ddx > 0) ? 3 : 2;
	v = (ddy > 0) ? 1 : 0;

    // Prioritize horizontal or vertical exploration depending on which distance is greater
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

/**
 * @brief Standard BFS pathfinding to compute the next single step towards a destination.
 * * Finds the shortest obstacle-free path to a cell adjacent to the destination.
 * Uses direction ordering heuristic to optimize queue exploration.
 */
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

    // If already adjacent to the destination, no move is needed
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

    // Initialize parent array with -1 (unvisited)
    i = 0;
	while (i < size)
		parent[i++] = -1;
	head = 0;
	tail = 0;
	cur = src.y * data->map_size + src.x;
	parent[cur] = cur;  // Mark source as its own parent
	queue[tail++] = cur;

    // Calculate directional heuristic priorities
    build_dir_order(src, dest, order);

    // Pathfinding loop
    while (head < tail) {
		cur = queue[head++];

        // Skip check for the initial starting node
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

        // Expand neighbors using the prioritized direction order
		i = 0;
		while (i < 4) {
			nx = (cur % data->map_size) + g_dx[order[i]];
			ny = (cur / data->map_size) + g_dy[order[i]];
			i++;

			if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
				continue ;

			ni = ny * data->map_size + nx;
			if (parent[ni] != -1)   // Already visited
				continue ;

			if (board[ni] != TILE_EMPTY)    // Obstacle or another player
				continue ;

			parent[ni] = cur;   // Track path lineage
			queue[tail++] = ni;
		}
	}
    free(queue);
    free(parent);
    return (step);
}

