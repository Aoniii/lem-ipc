#ifndef LEM_IPC_H
# define LEM_IPC_H

# include "libft.h"
# include "parser.h"

# include <stdbool.h>

int ft_printf(const char *format, ...);

typedef struct      s_data {
    unsigned int    map_size;
    unsigned int    ai;
    bool            spectator;
    bool            walls;
    bool            replay;
    bool            verbose;
}                   t_data;

#endif
