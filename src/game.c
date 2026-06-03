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
    unsigned int    board_size;
    t_pos           pos = {0};
    bool            show_display = (data->human || data->spectator);

    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (1);
    }

    if (show_display) {
        if (display_init(data) != 0) {
            //ipc_cleanup(data);
            ft_printf("lemipc: error: display init failed (terminal too small?)\n");
            return (1);
        }
    }

    board_size = data->map_size * data->map_size;
    snapshot = malloc(board_size);
    if (!snapshot) {
        if (show_display) display_destroy();
        //ipc_cleanup(data);
        ft_printf("lemipc: error: memory allocation failed\n");
        return (1);
    }

    if (!data->spectator) {
        if (player_place(data, &pos) == -1) {
            free(snapshot);
            if (show_display) display_destroy();
            //ipc_cleanup(data);
            ft_printf("lemipc: error: board is full\n");
            return (1);
        }
        sem_lock(data->sem_id);
        log_push(data, "[+] Team %d joined at (%d, %d)", data->team, pos.x + 1, pos.y + 1);
        sem_unlock(data->sem_id);
    }

    game_loop(data, snapshot, board_size, show_display, &pos);
    free(snapshot);
    if (show_display) display_destroy();
    //ipc_cleanup(data);
    return (0);
}

void    game_loop(t_data *data, unsigned char *snapshot, unsigned int size, bool show_display, t_pos *pos) {
    while (1) {
        if (show_display) {
            sem_lock(data->sem_id);
            ft_memcpy(snapshot,  board_get(data), size);
            sem_unlock(data->sem_id);
            display_render(data, snapshot, pos);
        }

        usleep(1000000 / FPS);
    }
}
