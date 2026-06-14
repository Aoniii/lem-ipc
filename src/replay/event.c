#include "display.h"
#include "lem-ipc.h"
#include "ipc.h"
#include <stdio.h>
#include <unistd.h>

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

