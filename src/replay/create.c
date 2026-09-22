#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "board.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"

/**
 * @brief Writes the replay start header: map size, then the initial board.
 *
 * The board dump records the walls so the replay can reconstruct them. It is
 * written here, while the shared memory is still private to the creator (ready
 * == 0), so the header is always the very first thing in the file: any other
 * player can only append events once ready == 1.
 */
static int replay_write_header(t_data *data, int fd) {
	char			line[MAX_MAP_SIZE * MAX_MAP_SIZE + 64];
	unsigned char	*board;
	unsigned int	size;
	unsigned int	i;
	int				len;

	board = board_get(data);
	size = data->map_size * data->map_size;

	// map size
	len = snprintf(line, sizeof(line), "MAP %u\nBOARD ", data->map_size);
	if (len < 0)
		return (-1);

	// Initial board : '1' = wall, '0' = empty; all on one line
	i = 0;
	while (i < size)
		line[len++] = (board[i++] == TILE_WALL) ? '1' : '0';
	line[len++] = '\n';

	return (write(fd, line, len) == len ? 0 : -1);
}

/**
 * @brief Initializes a new game replay file.
 *
 * This function retrieves the current working directory, ensures a "/replay"
 * folder exists, and generates a timestamped filename (e.g.,
 * game-YYYYMMDD-HHMMSS-PID.log). The absolute path is saved into the shared
 * memory for later use, and the header (map size + initial board) is written
 * right away.
 */
int create_replay_file(t_data *data) {
	t_shm_header	*header;
	char			cwd[PATH_MAX];
	char			dir[PATH_MAX + 64];
	int				fd;
	int				len;
	time_t			now;
	struct tm		*t;

	header = (t_shm_header *)data->shm_ptr;
	header->replay_path[0] = 0;

	// 1. Retrieve the absolute directory
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (-1);

	// 2. Create the replay folder (absolute path)
	len = snprintf(dir, sizeof(dir), "%s/replay", cwd);
	if (len < 0 || (size_t)len >= sizeof(dir))
		return (-1);
	if (mkdir(dir, 0755) == -1 && errno != EEXIST)
		return (-1);

	// 3. Generate the filename based on the date and time. The pid keeps the
	// name unique when two games happen to be created in the same second.
	now = time(NULL);
	t = localtime(&now);
	len = snprintf(header->replay_path, sizeof(header->replay_path),
		"%s/game-%04d%02d%02d-%02d%02d%02d-%d" FILE_EXTENSION,
		dir, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec, (int)getpid());
	if (len < 0 || (size_t)len >= sizeof(header->replay_path)) {
		header->replay_path[0] = '\0';
		return (-1);
	}

	// 4. Create the file (O_EXCL: never truncate someone else's replay)
	fd = open(header->replay_path, O_WRONLY | O_CREAT | O_EXCL, 0644);
	if (fd == -1) {
		header->replay_path[0] = 0;
		return (-1);
	}

	// 5. Write the header before anyone else can append to the file
	if (replay_write_header(data, fd) == -1) {
		close(fd);
		header->replay_path[0] = '\0';
		return (-1);
	}

	close(fd);
	return (0);
}
