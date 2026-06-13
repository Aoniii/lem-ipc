#ifndef REPLAY_H
# define REPLAY_H

# include <stdbool.h>

# define FILE_EXTENSION ".log"

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

int     create_replay_file(t_data *data);
int     replay_open(t_data *data, bool write_header);
void    replay_event(t_data *data, const char *name, t_pos pos);
void    replay_join(t_data *data, t_pos pos);
void    replay_quit(t_data *data, t_pos pos);
void    replay_move(t_data *data, t_pos old, int dx, int dy);

#endif
