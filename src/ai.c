#include "ai.h"
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
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
    if (res.found)
        player_move(data, pos, res.dx, res.dy);
}

void    ai_move(t_data *data, t_pos *pos) {
    static int      frame = 0;

    frame++;
    if (frame == FPS) {
        sem_lock(data->sem_id);
        if (data->ai == 1) ai_random_move(data, pos);
        if (data->ai == 2) ai_chase_move(data, pos);
        //add msg
        sem_unlock(data->sem_id);
        frame = 0;
    }
}
