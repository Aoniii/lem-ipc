#include <fcntl.h>
#include <unistd.h>
#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"

/**
 * @brief Opens the replay file in append mode.
 *
 * The header (map size + initial board) is written once by the creator in
 * create_replay_file(), before the game is announced as ready, so every other
 * player only ever appends events. Returns the fd, or -1 on failure.
 */
int replay_open(t_data *data) {
	t_shm_header	*header;

	header = (t_shm_header *)data->shm_ptr;
	if (header->replay_path[0] == '\0')
		return (-1);

	// No O_CREAT: the file must already exist with its header
	return (open(header->replay_path, O_WRONLY | O_APPEND));
}
