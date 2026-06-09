#ifndef MSG_H
# define MSG_H

#include <stdbool.h>

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

typedef struct  s_msg {
    long        mtype;
    int         x;
    int         y;
}               t_msg;

void    msg_send_target(t_data *data, t_pos target);
bool    msg_recv_target(t_data *data, t_pos *out);

#endif
