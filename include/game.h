#ifndef GAME_H
# define GAME_H

typedef struct s_data   t_data;

int     game_start(t_data *data);
void    game_loop(t_data *data, unsigned char *snapshot, unsigned int size);

#endif
