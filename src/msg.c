#include "msg.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <sys/msg.h>

/**
 * @brief Broadcasts an enemy target's coordinate position to all teammates.
 * * Uses the team ID as the message type (`mtype`) to ensure that only 
 * members of the same team will pick up this signal. Sends the packet 
 * asynchronously using `IPC_NOWAIT` to avoid blocking the player's loop 
 * if the kernel message buffer is full.
 */
void    msg_send_target(t_data *data, t_pos target) {
	t_msg   msg;

	msg.mtype = data->team; // Filter criteria: channel key unique to this team
	msg.x = target.x;
	msg.y = target.y;

    // Size sent strips out the mandatory initial 'long mtype' field size
	msgsnd(data->msg_id, &msg, sizeof(msg) - sizeof(long), IPC_NOWAIT);
}

/**
 * @brief Flushes the team's message queue to read the latest broadcasted target.
 * * Reads incoming packets non-blocking (`IPC_NOWAIT`) matching the current team ID.
 * Loops through all outstanding messages on the channel to process the newest entry,
 * ensuring that the team reacts to the most up-to-date tactical coordinates.
 */
bool    msg_recv_target(t_data *data, t_pos *out) {
	t_msg   msg;
	bool    found = false;

    // Drain the message queue channel completely to consume historical logs
	while (msgrcv(data->msg_id, &msg, sizeof(msg) - sizeof(long), data->team, IPC_NOWAIT) != -1) {
		out->x = msg.x;
		out->y = msg.y;
		found = true;   // Flag that an update has been found
	}
	return (found); // Returns true only if a packet matched our data channel filter
}
