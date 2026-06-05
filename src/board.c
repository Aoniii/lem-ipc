#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <stddef.h>

unsigned char   *board_get(t_data *data) {
    return ((unsigned char *)data->shm_ptr + sizeof(t_shm_header));
}

unsigned int    board_count(t_data *data, unsigned char value) {
    unsigned char   *board = board_get(data);
    unsigned int    size = data->map_size * data->map_size;
    unsigned int    i = 0;
    unsigned int    ret = 0;

    while (i < size) {
        if (board[i] == value)
            ret++;
        i++;
    }

    return (ret);
}

void    board_set_empty(t_data *data, t_pos pos) {
    unsigned char   *board = board_get(data);
    board[pos.y * data->map_size + pos.x] = TILE_EMPTY;
}
