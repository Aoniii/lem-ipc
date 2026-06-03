#ifndef GAME_H
# define GAME_H

# include <stdbool.h>

typedef struct s_data   t_data;
typedef struct s_logs   t_logs;
typedef struct s_pos   t_pos;

int     game_start(t_data *data);
void    game_loop(t_data *data, unsigned char *snapshot, unsigned int size, bool show_display, t_logs *logs, t_pos *pos);

#endif
