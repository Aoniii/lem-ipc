#include "display.h"
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

void    player_join(t_data *data, t_pos pos) {
    sem_lock(data->sem_id);
    log_push(data, "[+] Team %d joined at (%d, %d)", data->team, pos.x + 1, pos.y + 1);
    ((t_shm_header *)data->shm_ptr)->player_count++;
    sem_unlock(data->sem_id);
}

void    player_quit(t_data *data) {
    sem_lock(data->sem_id);
    log_push(data, "[-] Team %d left", data->team);
    sem_unlock(data->sem_id);
}

bool    player_move(t_data *data, t_pos *pos, int dx, int dy) {
    unsigned char   *board;
    int             nx = pos->x + dx;
    int             ny = pos->y + dy;

    if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
        return (false);

    sem_lock(data->sem_id);
    board = board_get(data);
    if (board[ny * data->map_size + nx] != TILE_EMPTY) {
        sem_unlock(data->sem_id);
        return (false);
    }

    board[pos->y * data->map_size + pos->x] = TILE_EMPTY;
    board[ny * data->map_size + nx] = data->team;
    pos->x = nx;
    pos->y = ny;
    sem_unlock(data->sem_id);
    return (true);
}
