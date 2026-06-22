#ifndef IPC_H
# define IPC_H

# include <linux/limits.h>
# include <sys/ipc.h>
# include <sys/sem.h>
# include <stdbool.h>
# include "lem-ipc.h"

# define IPC_PATH   "/tmp/lemipc"
# define IPC_PROJ   '*'
# define IPC_PERMS  0600

# define SEM_MAX_RETRY  1000
# define SHM_MAX_RETRY  1000

# define LOG_COUNT  10
# define LOG_WIDTH  (MAX_MAP_SIZE * 2)

typedef struct s_data   t_data;

// semun is not defined by glibc.
union               semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
};

typedef struct      s_shm_header {
    unsigned int    map_size;
    unsigned int    player_count;
    char            logs[LOG_COUNT][LOG_WIDTH];
    int             log_count;
    int             log_head;
    char            replay_path[PATH_MAX + 128];
    long            start_ms;
    int             event_id;
    int             kill[MAX_TEAM + 1];
    int             winner_team;
    volatile int    ready;
    volatile int    running;
}                   t_shm_header;

// ipc.c
int     ipc_init(t_data *data);
void    ipc_cleanup(t_data *data);

// sem.c
int     sem_setup(key_t key, int *sem_id, bool *is_creator);
void    sem_lock(int sem_id);
void    sem_unlock(int sem_id);

#endif
