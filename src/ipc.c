#include "ipc.h"
#include "lem-ipc.h"
#include "libft.h"
#include <errno.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

static int  create_ipc_file(void) {
    int fd = open(IPC_PATH, O_CREAT | O_RDONLY, IPC_PERMS);
    if (fd != -1)
        close(fd);
    return (fd);
}

static int  ipc_create(t_data *data, key_t key) {
    t_shm_header    *header;
    size_t          size = sizeof(t_shm_header) + data->map_size * data->map_size;

    data->shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->shm_id == -1) {
        if (errno == EEXIST)
            return (1);
        return (-1);
    }

    if (sem_setup(key, &data->sem_id, &data->is_first) == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        return (-1);
    }

    data->msg_id = msgget(key, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->msg_id == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        semctl(data->sem_id, 0, IPC_RMID);
        return (-1);
    }

    data->shm_ptr = shmat(data->shm_id, NULL, 0);
    if (data->shm_ptr == (void *)-1)
        return (-1);

    ft_memset(data->shm_ptr, 0, size);
    header = (t_shm_header *)data->shm_ptr;
    header->map_size = data->map_size;
    header->player_count = 0;
    header->running = 1;
    header->ready = 1;
    data->is_first = true;

    return (0);
}

static int  shm_wait_ready(t_shm_header *header) {
    int retry = 0;

    while (retry < SHM_MAX_RETRY) {
        if (header->ready == 1)
            return (0);
        usleep(1000);
        retry++;
    }
    return (-1);
}

static int  ipc_attach(t_data *data, key_t key) {
    t_shm_header    *header;
    bool            creator;

    data->shm_id = shmget(key, 0, IPC_PERMS);
    if (data->shm_id == -1)
        return (-1);

    if (sem_setup(key, &data->sem_id, &creator) == -1)
        return (-1);

    data->msg_id = msgget(key, IPC_PERMS);
    if (data->msg_id == -1)
        return (-1);

    data->shm_ptr = shmat(data->shm_id, NULL, 0);
    if (data->shm_ptr == (void *) -1)
        return (-1);

    header = (t_shm_header *)data->shm_ptr;
    if (shm_wait_ready(header) == -1)
        return (-1);
    data->map_size = header->map_size;
    data->is_first = false;

    return (0);
}

int ipc_init(t_data *data) {
    key_t   key;

    if (create_ipc_file() == -1)
        return (-1);
    key = ftok(IPC_PATH, IPC_PROJ);
    if (key == -1)
        return (-1);

    int ret = ipc_create(data, key);
    if (ret == 1)
        return (ipc_attach(data, key));

    return (ret);
}

void    ipc_cleanup(t_data *data) {
    t_shm_header    *header;
    bool            last;

    if (!data->shm_ptr || data->shm_ptr == (void *)-1)
        return;

    header = (t_shm_header *)data->shm_ptr;
    last = false;

    sem_lock(data->sem_id);
    if (header->player_count > 0) header->player_count--;
    if (header->player_count == 0) last = true;
    sem_unlock(data->sem_id);

    shmdt(data->shm_ptr);
    if (last) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        semctl(data->sem_id, 0, IPC_RMID);
        msgctl(data->msg_id, IPC_RMID, NULL);
        unlink(IPC_PATH);
    }
}
