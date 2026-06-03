#ifndef DISPLAY_H
# define DISPLAY_H

# define MARGIN     7

# define BLINK_TIME 400000
# define BLACK_ATTR 64

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

int     display_init(t_data *data);
void    display_render(t_data *data, unsigned char *snapshot, t_pos *pos);
void    display_destroy(void);

void    log_push(t_data *data, const char *fmt, ...);

#endif
