#ifndef DISPLAY_H
# define DISPLAY_H

# define MARGIN     7

# define BLINK_TIME 400000
# define BLACK_ATTR 64

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

// display.c
int     display_init(t_data *data);
void    display_render(t_data *data, unsigned char *snapshot, t_pos *pos);
void    display_destroy(void);
void    init_team_colors(void);
void    draw_border(unsigned int height, unsigned int start_y);

// log.c
void    log_push(t_data *data, const char *fmt, ...);
void    verbose_log(t_data *data, const char *fmt, ...);

#endif
