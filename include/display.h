#ifndef DISPLAY_H
# define DISPLAY_H

typedef struct s_data   t_data;

int     display_init(t_data *data);
void    display_render(t_data *data, unsigned char *snapshot);
void    display_destroy(void);

#endif
