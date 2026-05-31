#include "ipc.h"
#include "lem-ipc.h"
#include "print.h"

int game_start(t_data *data) {
    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (-1);
    }

    return (0);
}
