#include "ipc.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

static int  sem_wait_ready(int sem_id) {
    union semun     arg;
    struct semid_ds buf;
    int             retry = 0;

    arg.buf = &buf;
    while (retry < SEM_MAX_RETRY) {
        if (semctl(sem_id, 0, IPC_STAT, arg) == -1)
            return (-1);
        if (buf.sem_otime != 0)
            return (0);
        usleep(1000);
        retry++;
    }
    return (-1);
}

static int  sem_create(int sem_id) {
    union semun     arg;
    struct sembuf   op;

    arg.val = 0;
    if (semctl(sem_id, 0, SETVAL, arg) == -1)
        return (-1);
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
        return (-1);
    return (0);
}

int sem_setup(key_t key, int *sem_id, bool *is_creator) {
    *sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (*sem_id != -1) {
        *is_creator = true;
        return (sem_create(*sem_id));
    }

    if (errno != EEXIST)
        return (-1);

    *is_creator = false;
    *sem_id = semget(key, 1, IPC_PERMS);
    if (*sem_id == -1)
        return (-1);

    return (sem_wait_ready(*sem_id));
}

void    sem_lock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = SEM_UNDO;
    semop(sem_id, &op, 1);
}

void    sem_unlock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;
    semop(sem_id, &op, 1);
}
