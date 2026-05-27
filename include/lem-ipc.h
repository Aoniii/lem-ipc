#ifndef LEM_IPC_H
# define LEM_IPC_H

# include <stdbool.h>

# define DEFAULT_MAP_SIZE   10
# define DEFAULT_AI_LEVEL   1
# define MIN_MAP_SIZE       5
# define MAX_MAP_SIZE       25
# define MAX_AI_LEVEL       3

typedef struct      s_data {
    unsigned int    map_size;
    unsigned int    ai;
    bool            spectator;
    bool            walls;
    bool            replay;
    bool            verbose;
}                   t_data;

#endif
