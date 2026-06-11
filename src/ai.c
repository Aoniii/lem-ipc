#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
#include "msg.h"
#include <stdlib.h>

/**
 * @brief Moves the AI in a completely random direction.
 * Chooses a direction at random (Up, Down, Left, Right) and attempts to move.
 * If a collision occurs, it retries up to RETRY_MOVE times.
 */
static void ai_random_move(t_data *data, t_pos *pos) {
    // Directional offset arrays (dx/dy) for: Up, Down, Left, Right
    static const int    dx[4] = {0, 0, -1, 1};
    static const int    dy[4] = {-1, 1, 0, 0};

    int r = 0;
    for (int i = 0; i < RETRY_MOVE; i++) {
        r = rand() % 4; // Select a random direction index
        // If the move is valid and succeeds, stop trying
        if (player_move(data, pos, dx[r], dy[r])) break;
    }
}

/**
 * @brief "Solo" chase behavior using BFS.
 * Uses a Breadth-First Search (BFS) algorithm to locate the closest valid target
 * from its own position, then takes a single step toward it.
 */
static void ai_chase_move(t_data *data, t_pos *pos) {
    t_pos   me[1];
    me[0] = *pos;

    // Run multi-source BFS starting only with this AI's position
    t_bfs_result    res = ai_bfs_multi(data, me, 1);
    if (res.found) {
        // Calculate the next step (displacement vector) toward the target
        t_pos   move = ai_step_to(data, *pos, res.target);
        player_move(data, pos, move.x, move.y);
    }
}

/**
 * @brief Checks if a target coordinate is valid for an attack.
 * A target is valid if it lies within the map boundaries, is not empty space 
 * or a wall, and belongs to an opposing team.
 */
static bool    target_is_valid(t_data *data, t_pos target) {
	unsigned char   tile;

    // Map boundaries check
	if (target.x < 0 || target.x >= (int)data->map_size || target.y < 0 || target.y >= (int)data->map_size)
		return (false);

    // Fetch the tile from the board
	tile = board_get(data)[target.y * data->map_size + target.x];

    // Only valid if it's not empty, not a wall, and belongs to an enemy team
	return (tile != TILE_EMPTY && tile != TILE_WALL && tile != data->team);
}

/**
 * @brief Coordinated team chase behavior.
 * 1. Checks if a teammate broadcasted a target via messages. If valid, 
 *    the AI pursues it and relays the message.
 * 2. Otherwise, runs a multi-source BFS using the entire team's coordinates 
 *    to find the nearest enemy, moves toward them, and broadcasts the new target.
 */
static void ai_coordination_move(t_data *data, t_pos *pos) {
    t_pos   team[MAX_MAP_SIZE * MAX_MAP_SIZE];
    t_pos   out;

    // Strategy A: Follow a target received via message
    if (msg_recv_target(data, &out)) {
        if (target_is_valid(data, out)) {
            t_pos   move = ai_step_to(data, *pos, out);
            player_move(data, pos, move.x, move.y);
            msg_send_target(data, out);
            return ;
        }
    }

    // Strategy B: No valid message received; hunt for a target as a team
    int             n = team_positions(data, team, MAX_MAP_SIZE * MAX_MAP_SIZE);
	t_bfs_result    res = ai_bfs_multi(data, team, n);
    if (res.found) {
        t_pos   move = ai_step_to(data, *pos, res.target);
        player_move(data, pos, move.x, move.y);
        msg_send_target(data, res.target);  // Share the found target with the team
    }
}

/**
 * @brief Main entry point for AI movement routing.
 * Dispatches the entity to the appropriate movement routine based on its configuration.
 */
void    ai_move(t_data *data, t_pos *pos) {
    if (data->ai == 1) ai_random_move(data, pos);
    else if (data->ai == 2) ai_chase_move(data, pos);
    else if (data->ai == 3) ai_coordination_move(data, pos);
}
