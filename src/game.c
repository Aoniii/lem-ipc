#include "ipc.h"
#include "lem-ipc.h"
#include "print.h"
#include "board.h"

int game_start(t_data *data) {
    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (-1);
    }

    ft_printf("count: %i\n", board_count(data, 0));

    return (0);
}
