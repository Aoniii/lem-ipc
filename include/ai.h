#ifndef AI_H
# define AI_H

# include "lem-ipc.h"

# define RETRY_MOVE 5

typedef struct  s_bfs_result {
    bool        found;
    int         dx;
    int         dy;
    t_pos       target;
}               t_bfs_result;

void            ai_move(t_data *data, t_pos *pos);
t_bfs_result    ai_bfs_multi(t_data *data, t_pos *sources, int count);

#endif
