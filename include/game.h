#ifndef GAME_H
# define GAME_H

# include <stdbool.h>

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

int     game_start(t_data *data);
int     game_loop(t_data *data, bool show_display, t_pos *pos);

#endif
