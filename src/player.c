#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
#include "board.h"
#include <stdlib.h>

int player_place(t_data *data, t_pos *pos) {
    sem_lock(data->sem_id);

    unsigned int    count = board_count(data, TILE_EMPTY);
    if (count == 0) {
        sem_unlock(data->sem_id);
        return (-1);
    }

    unsigned char   *board = board_get(data);
    unsigned int    rdm = rand() % count;
    unsigned int    i = 0;
    while (i < data->map_size * data->map_size) {
        if (board[i] == TILE_EMPTY) {
            if (rdm == 0) break;
            rdm--;
        }
        i++;
    }

    board[i] = data->team;
    pos->x = i % data->map_size;
    pos->y = i / data->map_size;
    sem_unlock(data->sem_id);
    return (0);
}
