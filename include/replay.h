#ifndef REPLAY_H
# define REPLAY_H

# include <stdbool.h>

# define FILE_EXTENSION ".log"

typedef struct s_data   t_data;

int create_replay_file(t_data *data);
int replay_open(t_data *data, bool write_header);

#endif
