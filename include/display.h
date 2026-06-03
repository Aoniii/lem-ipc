#ifndef DISPLAY_H
# define DISPLAY_H

# include "lem-ipc.h"

# define MARGIN     7
# define LOG_COUNT  5
# define LOG_WIDTH  MAX_MAP_SIZE * 2

# define BLINK_TIME 400000
# define BLACK_ATTR 64

typedef struct  s_logs {
    char        lines[LOG_COUNT][LOG_COUNT];
    int         count;
    int         head;
}               t_logs;

int     display_init(t_data *data);
void    display_render(t_data *data, unsigned char *snapshot, t_logs *logs, t_pos *pos);
void    display_destroy(void);

void    log_push(t_logs *logs, const char *fmt, ...);

#endif
