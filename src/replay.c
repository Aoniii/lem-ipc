#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"
#include "board.h"
#include "libft.h"
#include "print.h"
#include "libft.h"
#include "get_next_line_bonus.h"
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

/**
 * @brief Logs a event to the replay file.
 * Format: <ms> <id> <name> <team> <x> <y>
 * Must be called while holding the semaphore lock.
 */
void    replay_event(t_data *data, const char *name, t_pos pos) {
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
	len = snprintf(line, sizeof(line), "%ld %u %s %d %d %d\n", elapsed, id, name, data->team, pos.x, pos.y);
	write(data->replay_fd, line, len);
}

void    replay_join(t_data *data, t_pos pos) { replay_event(data, "JOIN", pos); }
void    replay_quit(t_data *data, t_pos pos) { replay_event(data, "QUIT", pos); }

/**
 * @brief Logs a MOVE event to the replay file.
 * Format: <ms> <id> MOVE <team> <x> <y> <dx> <dy>
 * Stores the position before moving and the direction.
 * Must be called while holding the semaphore lock.
 */
void    replay_move(t_data *data, t_pos old, int dx, int dy) {
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
	len = snprintf(line, sizeof(line), "%ld %u MOVE %d %d %d %d %d\n", elapsed, id, data->team, old.x, old.y, dx, dy);
	write(data->replay_fd, line, len);
}

static int replay_play(t_replay *replay) {
    (void)replay;
    return (0);
}

static void free_split(char **split) {
	int i;

	if (!split)
		return ;

	i = 0;
	while (split[i])
		free(split[i++]);
	free(split);
}

// parse “MAP <size>” and “BOARD <0/1...>”, set map_size and initial_board
static int  parse_header(t_replay *replay, int fd) {
	char    *line;
	char    **parts;
	int	    i;

	// --- line MAP ---
	line = get_next_line(fd);
	if (!line)
		return (-1);

	parts = ft_split(line, ' ');
	free(line);
	if (!parts || !parts[0] || !parts[1] || ft_strncmp(parts[0], "MAP", 4) != 0) {
        free_split(parts);
        return (-1);
    }

	replay->map_size = ft_atoi(parts[1]);
	free_split(parts);
	if (replay->map_size < MIN_MAP_SIZE || replay->map_size > MAX_MAP_SIZE)
		return (-1);

	// --- line BOARD ---
	line = get_next_line(fd);
	if (!line)
		return (-1);

	parts = ft_split(line, ' ');
	free(line);
	if (!parts || !parts[0] || !parts[1] || ft_strncmp(parts[0], "BOARD", 6) != 0) {
        free_split(parts);
        return (-1);
    }

    // check that the string is exactly map_size * map_size in length and consists of 0s and 1s
	if (ft_strlen(parts[1]) != replay->map_size * replay->map_size) {
        free_split(parts);
        return (-1);
    }
	replay->initial_board = malloc(replay->map_size * replay->map_size);
	if (!replay->initial_board) {
        free_split(parts);
        return (-1);
    }

	i = 0;
	while (parts[1][i]) {
		if (parts[1][i] == '0') {
			replay->initial_board[i] = TILE_EMPTY;
        } else if (parts[1][i] == '1') {
			replay->initial_board[i] = TILE_WALL;
        } else {
            free_split(parts);
            return (-1);
        }
		i++;
	}

	free_split(parts);
	return (0);
}

static int replay_parse(t_replay *replay, int fd) {
    parse_header(replay, fd);
    return (0);
}

static void replay_free(t_replay *replay) {
    (void)replay;
}

int replay_start(char *filename) {
    t_replay    replay;
    int         fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
        return (-1);

    ft_memset(&replay, 0, sizeof(t_replay));
    if (replay_parse(&replay, fd) == -1) {
        close(fd);
        replay_free(&replay);
        ft_printf("lemipc: error: failed to parse the replay file\n");
        return (-1);
    }

    close(fd);
    if (replay_play(&replay) == -1) {
        replay_free(&replay);
		ft_printf("lemipc: error: unplayable event in replay\n");
        return (-1);
    }

    replay_free(&replay);
    return (0);
}
