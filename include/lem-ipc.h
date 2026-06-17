#ifndef LEM_IPC_H
# define LEM_IPC_H

# include <stdbool.h>
# include <signal.h>

# define STRINGIFY(x)   #x
# define STR(x)         STRINGIFY(x)

# define DEFAULT_MAP_SIZE   10
# define MIN_MAP_SIZE       5
# define MAX_MAP_SIZE       25
# define DEFAULT_AI_LEVEL   3
# define MIN_AI_LEVEL       1
# define MAX_AI_LEVEL       3

# define MAX_TEAM   16

# define TILE_EMPTY 0
# define TILE_WALL  255

# define FPS    30

extern volatile sig_atomic_t    g_stop;

typedef struct      s_data {
    unsigned int    map_size;
    unsigned int    ai;
    bool            spectator;
    bool            human;
    bool            walls;
    char            *replay;
    bool            verbose;
    unsigned char   team;
    int             shm_id;
    int             sem_id;
    int             msg_id;
    void*           shm_ptr;
    bool            is_first;
    bool            is_alive;
    int             replay_fd;
}                   t_data;

typedef struct  s_pos {
    int         x;
    int         y;
}               t_pos;

void    setup_signals(void);
long    now_ms(void);
int     team_with_most_kills(int *kills, int *nb);

#endif
