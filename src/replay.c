#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"
#include "board.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * @brief Initializes a new game replay file.
 * * This function retrieves the current working directory, ensures a "/replay" 
 * folder exists, and generates a timestamped filename (e.g., game-YYYYMMDD-HHMMSS.log).
 * The absolute path is saved into the shared memory for later use.
 */
int create_replay_file(t_data *data) {
    t_shm_header    *header;
    char            cwd[PATH_MAX];
    char            dir[PATH_MAX + 64];
    int             fd;
    time_t          now;
    struct tm       *t;

    header = (t_shm_header *)data->shm_ptr;

    // 1. Retrieve the absolute directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (-1);

    // 2. Create the replay folder (absolute path)
    snprintf(dir, sizeof(dir), "%s/replay", cwd);
    mkdir(dir, 0755);

    // 3. Generate the filename based on the date and time
    now = time(NULL);
    t = localtime(&now);
    snprintf(header->replay_path, sizeof(header->replay_path),
        "%s/replay/game-%04d%02d%02d-%02d%02d%02d.log",
        cwd, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    // 4. Create the file
    fd = open(header->replay_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return (-1);
    close(fd);
    return (0);
}

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
    write(fd, &c, 1);
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

/**
 * @brief Logs a JOIN event to the replay file.
 * Format: <ms> <id> JOIN <team> <x> <y>
 * Must be called while holding the semaphore lock.
 */
void    replay_join(t_data *data, t_pos pos) {
	t_shm_header    *header;
	char            line[64];
	long            elapsed;
	unsigned int    id;
	int             len;

	if (data->replay_fd == -1)
		return ;
	header = (t_shm_header *)data->shm_ptr;
	elapsed = now_ms() - header->start_ms;
	id = header->event_id++;
	len = snprintf(line, sizeof(line), "%ld %u JOIN %d %d %d\n", elapsed, id, data->team, pos.x, pos.y);
	write(data->replay_fd, line, len);
}
