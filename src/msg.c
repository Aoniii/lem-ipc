#include "msg.h"
#include "ipc.h"
#include "lem-ipc.h"
#include <sys/msg.h>

void    msg_send_target(t_data *data, t_pos target) {
	t_msg   msg;

	msg.mtype = data->team;
	msg.x = target.x;
	msg.y = target.y;
	msgsnd(data->msg_id, &msg, sizeof(msg) - sizeof(long), IPC_NOWAIT);
}

bool    msg_recv_target(t_data *data, t_pos *out) {
	t_msg   msg;
	bool    found = false;

	while (msgrcv(data->msg_id, &msg, sizeof(msg) - sizeof(long), data->team, IPC_NOWAIT) != -1) {
		out->x = msg.x;
		out->y = msg.y;
		found = true;
	}
	return (found);
}
