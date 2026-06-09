#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
#include "msg.h"
#include <stdlib.h>

static void ai_random_move(t_data *data, t_pos *pos) {
    static const int    dx[4] = {0, 0, -1, 1};
    static const int    dy[4] = {-1, 1, 0, 0};

    int r = 0;
    for (int i = 0; i < RETRY_MOVE; i++) {
        r = rand() % 4;
        if (player_move(data, pos, dx[r], dy[r])) break;
    }
}

static void ai_chase_move(t_data *data, t_pos *pos) {
    t_pos   me[1];
    me[0] = *pos;

    t_bfs_result    res = ai_bfs_multi(data, me, 1);
    if (res.found) {
        t_pos   move = ai_step_to(data, *pos, res.target);
        player_move(data, pos, move.x, move.y);
    }
}

static bool    target_is_valid(t_data *data, t_pos target) {
	unsigned char   tile;

	if (target.x < 0 || target.x >= (int)data->map_size || target.y < 0 || target.y >= (int)data->map_size)
		return (false);
	tile = board_get(data)[target.y * data->map_size + target.x];
	return (tile != TILE_EMPTY && tile != TILE_WALL && tile != data->team);
}

static void ai_coordination_move(t_data *data, t_pos *pos) {
    t_pos   team[MAX_MAP_SIZE * MAX_MAP_SIZE];
    t_pos   out;

    if (msg_recv_target(data, &out)) {
        if (target_is_valid(data, out)) {
            t_pos   move = ai_step_to(data, *pos, out);
            player_move(data, pos, move.x, move.y);
            msg_send_target(data, out);
            return ;
        }
    }

    int             n = team_positions(data, team, MAX_MAP_SIZE * MAX_MAP_SIZE);
	t_bfs_result    res = ai_bfs_multi(data, team, n);
    if (res.found) {
        t_pos   move = ai_step_to(data, *pos, res.target);
        player_move(data, pos, move.x, move.y);
        msg_send_target(data, res.target);
    }
}

void    ai_move(t_data *data, t_pos *pos) {
    if (data->ai == 1) ai_random_move(data, pos);
    else if (data->ai == 2) ai_chase_move(data, pos);
    else if (data->ai == 3) ai_coordination_move(data, pos);
}
