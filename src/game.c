#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
#include "print.h"
#include "board.h"

int game_start(t_data *data) {
    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (-1);
    }

    t_pos   pos;
    if (player_place(data, &pos) == -1) {
        //print error
        return (-1);
    }

    return (0);
}
