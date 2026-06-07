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
    t_pos   nearest;
    int     r = rand() % 2;

    sem_lock(data->sem_id);
    nearest = player_nearest_get(data, pos);
    if (nearest.x != 0 || nearest.y != 0) {
        nearest.x = nearest.x - pos->x;
        nearest.y = nearest.y - pos->y;
        if (nearest.x > 0) nearest.x = 1;
        else if (nearest.x < 0) nearest.x = -1;
        if (nearest.y > 0) nearest.y = 1;
        else if (nearest.y < 0) nearest.y = -1;
    }
    sem_unlock(data->sem_id);

    if ((r == 0 || nearest.y == 0) && nearest.x != 0) player_move(data, pos, nearest.x, 0);
    else player_move(data, pos, 0, nearest.y);
}

void    ai_move(t_data *data, t_pos *pos) {
    static int      frame = 0;

    frame++;
    if (frame == FPS) {
        if (data->ai == 1) ai_random_move(data, pos);
        if (data->ai == 2) ai_chase_move(data, pos);
        //add msg
        frame = 0;
    }
}
