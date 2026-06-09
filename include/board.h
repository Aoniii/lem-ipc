#ifndef BOARD_H
# define BOARD_H

# include <stdbool.h>

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

unsigned char   *board_get(t_data *data);
unsigned int    board_count(t_data *data, unsigned char value);
void            board_set_empty(t_data *data, t_pos pos);
int             team_positions(t_data *data, t_pos *out, int max);
bool            is_circled(t_data *data, t_pos *pos);

#endif
