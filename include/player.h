#ifndef PLAYER_H
# define PLAYER_H

# include <stdbool.h>

typedef struct s_data   t_data;
typedef struct s_pos   t_pos;

int     player_place(t_data *data, t_pos *pos);
void    player_join(t_data *data, t_pos pos);
void    player_quit(t_data *data, t_pos pos);
bool    player_move(t_data *data, t_pos *pos, int dx, int dy);

#endif
