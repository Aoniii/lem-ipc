#include "ipc.h"
#include "lem-ipc.h"
#include <stddef.h>

unsigned char   *board_get(t_data *data) {
    return ((unsigned char *)data->shm_ptr + sizeof(t_shm_header));
}

int board_count(t_data *data, unsigned char value) {
    unsigned char   *board = board_get(data);
    int             size = data->map_size * data->map_size;
    int             i = 0;
    int             ret = 0;

    while (i < size) {
        if (board[i] == value)
            ret++;
        i++;
    }

    return (ret);
}
