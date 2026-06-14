#include "replay.h"
#include "lem-ipc.h"
#include "board.h"
#include "ipc.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @brief Writes the replay start header: map size, Initial board.
 * The board dump records the walls so the replay can reconstruct them.
 */
static void replay_write_header(t_data *data, int fd) {
    unsigned char   *board;
    char            line[32];
    char            c;
    unsigned int    i;
    int             len;

    board = board_get(data);

    // map size
    len = snprintf(line, sizeof(line), "MAP %u\n", data->map_size);
    write(fd, line, len);

    // Initial board : '1' = wall, '0' = empty; all on one line
    write(fd, "BOARD ", 6);
    i = 0;
    while (i < data->map_size * data->map_size) {
        c = (board[i] == TILE_WALL) ? '1' : '0';
        write(fd, &c, 1);
        i++;
    }
    write(fd, "\n", 1);
}

/**
 * @brief Opens the replay file in append mode.
 * If write_header is true, also writes the start header (map size, Initializes
 * board with walls, start timestamp). Return the fd, or -1 on failure.
 */
int replay_open(t_data *data, bool write_header) {
    t_shm_header    *header;
    int             fd;

    header = (t_shm_header *)data->shm_ptr;
    if (header->replay_path[0] == '\0')
        return (-1);

    fd = open(header->replay_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd == -1)
        return (-1);

    if (write_header)
        replay_write_header(data, fd);
    return (fd);
}
