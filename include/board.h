#ifndef BOARD_H
# define BOARD_H

typedef struct s_data   t_data;

unsigned char   *board_get(t_data *data);
int             board_count(t_data *data, unsigned char value);

#endif
