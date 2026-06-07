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

void    ai_move(t_data *data, t_pos *pos) {
    static int      frame = 0;

    frame++;
    if (frame == FPS) {
        if (data->ai == 1) ai_random_move(data, pos);
        frame = 0;
    }
}
