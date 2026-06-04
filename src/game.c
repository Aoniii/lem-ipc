#include "display.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "libft.h"
#include "player.h"
#include "print.h"
#include "board.h"
#include "game.h"
#include "ipc.h"
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

int game_start(t_data *data) {
    t_pos           pos = {0};
    bool            show_display = (data->human || data->spectator);

    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (1);
    }

    if (show_display) {
        if (display_init(data) != 0) {
            ipc_cleanup(data);
            ft_printf("lemipc: error: display init failed (terminal too small?)\n");
            return (1);
        }
    }

    if (!data->spectator) {
        if (player_place(data, &pos) == -1) {
            if (show_display) display_destroy();
            ipc_cleanup(data);
            ft_printf("lemipc: error: board is full\n");
            return (1);
        }
        player_join(data, pos);
    }

    data->is_alive = true;
    int ret = game_loop(data, show_display, &pos);
    if (show_display) display_destroy();
    ipc_cleanup(data);
    return (ret);
}

int game_loop(t_data *data, bool show_display, t_pos *pos) {
    t_shm_header    *header = (t_shm_header *)data->shm_ptr;
	int             running = 1;
    unsigned int    size = data->map_size * data->map_size;
    unsigned char   *snapshot = NULL;

    if (show_display) {
        snapshot = malloc(size);
        if (!snapshot) {
            ft_printf("lemipc: error: memory allocation failed\n");
            return (1);
        }
    }

    while (running && data->is_alive && !g_stop) {
        sem_lock(data->sem_id);

        running = header->running;
        if (!running) {
            sem_unlock(data->sem_id);
            break ;
        }

        if (show_display) ft_memcpy(snapshot,  board_get(data), size);
        sem_unlock(data->sem_id);
        if (show_display) {
            display_render(data, snapshot, pos);
            int ch = getch();
            if (ch == 'q' || ch == 'Q') break ;
        }

        usleep(1000000 / FPS);
    }

    if (!data->spectator) player_quit(data);
    if (show_display) free(snapshot);
    return (0);
}
