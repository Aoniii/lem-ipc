#include "display.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "libft.h"
#include "player.h"
#include "print.h"
#include "board.h"
#include "game.h"
#include "ipc.h"
#include <stdlib.h>
#include <unistd.h>

int game_start(t_data *data) {
    unsigned char   *snapshot;
    t_pos           pos;
    unsigned int    board_size;

    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (1);
    }

    if (display_init(data) != 0) {
        //ipc_cleanup(data);
        //print error
        return (1);
    }

    board_size = data->map_size * data->map_size;
    snapshot = malloc(board_size);
    if (!snapshot) {
        display_destroy();
        //ipc_cleanup(data);
        //print error
        return (1);
    }

    if (!data->spectator) {
        if (player_place(data, &pos) == -1) {
            free(snapshot);
            display_destroy();
            //ipc_cleanup(data);
            //print error;
            return (1);
        }
    }

    game_loop(data, snapshot, board_size);
    free(snapshot);
    display_destroy();
    //ipc_cleanup(data);
    return (0);
}

void    game_loop(t_data *data, unsigned char *snapshot, unsigned int size) {
    while (1) {
        sem_lock(data->sem_id);
        ft_memcpy(snapshot,  board_get(data), size);
        sem_unlock(data->sem_id);
        display_render(data, snapshot);
        usleep(1000000/FPS);
    }
}
