#include "ipc.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

/**
 * @brief Waits until a newly created semaphore is initialized.
 * Creating a semaphore and setting its value are two separate calls, so
 * joiners wait for sem_otime to become non-zero (set by the creator's
 * first semop) before using it.
 */
static int  sem_wait_ready(int sem_id) {
    union semun     arg;
    struct semid_ds buf;
    int             retry = 0;

    arg.buf = &buf;
    while (retry < SEM_MAX_RETRY) {
        // Read the semaphore's kernel metadata
        if (semctl(sem_id, 0, IPC_STAT, arg) == -1)
            return (-1);

        // sem_otime turns non-zero exactly when the first semop() is successfully committed
        if (buf.sem_otime != 0)
            return (0);

        usleep(1000);   // Back off to preserve CPU overhead
        retry++;
    }
    return (-1);
}

/**
 * @brief Initializes the semaphore value and marks it ready.
 * Sets the value to 0, then a +1 semop brings it to 1 and sets sem_otime
 * (which is what joiners wait on).
 */
static int  sem_create(int sem_id) {
    union semun     arg;
    struct sembuf   op;

    arg.val = 0;
    if (semctl(sem_id, 0, SETVAL, arg) == -1)
        return (-1);

    // +1 op also sets sem_otime, signaling readiness
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
        return (-1);
    return (0);
}

/**
 * @brief Creates the semaphore, or attaches to it if it already exists.
 * The creator initializes it; everyone else waits for it to be ready.
 */
int sem_setup(key_t key, int *sem_id, bool *is_creator) {
    // Attempt exclusive creation
    *sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (*sem_id != -1) {
        *is_creator = true;
        return (sem_create(*sem_id));
    }

    // If error is anything other than "already exists", fail immediately
    if (errno != EEXIST)
        return (-1);

    // Client process pathway: attach to the existing instance
    *is_creator = false;
    *sem_id = semget(key, 1, IPC_PERMS);
    if (*sem_id == -1)
        return (-1);

    // Wait for the creator to finish initializing.
    return (sem_wait_ready(*sem_id));
}

/**
 * @brief Acquires the lock (P operation).
 * Decrements the semaphore; blocks if it's already at 0. SEM_UNDO releases
 * the lock automatically if the process dies while holding it.
 */
void    sem_lock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = SEM_UNDO;  // Auto-cleanup tracking flag activated
    semop(sem_id, &op, 1);
}

/**
 * @brief Releases the lock (V operation).
 * Increments the semaphore, waking a waiting process.
 */
void    sem_unlock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = SEM_UNDO;  // Auto-cleanup tracking flag activated
    semop(sem_id, &op, 1);
}
