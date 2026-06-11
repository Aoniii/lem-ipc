#include "ipc.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

/**
 * @brief Safety polling loop that checks if a newly created semaphore is initialized.
 * * System V semaphores have a historic flaw: creating a semaphore and initializing 
 * its value are two separate, non-atomic system calls. This function forces 
 * joining processes to wait until the creator has executed its first `semop()`, 
 * which updates the kernel tracking field `sem_otime` to a non-zero value.
 */
static int  sem_wait_ready(int sem_id) {
    union semun     arg;
    struct semid_ds buf;
    int             retry = 0;

    arg.buf = &buf;
    while (retry < SEM_MAX_RETRY) {
        // Fetch current kernel metadata about the semaphore array
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
 * @brief Performs the initial value settings and initial operational wake-up.
 * Sets the internal resource counter value to 0, then increments it atomically 
 * to 1 via a dummy `semop` operation to unlock the barrier for peers.
 */
static int  sem_create(int sem_id) {
    union semun     arg;
    struct sembuf   op;

    arg.val = 0;
    if (semctl(sem_id, 0, SETVAL, arg) == -1)
        return (-1);

    // Execute a dummy operation to switch sem_otime to non-zero, signaling readiness
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
        return (-1);
    return (0);
}

/**
 * @brief Thread-safe master routing initialization manager for the lock structure.
 * * Tries to exclusively create the semaphore array block. If it succeeds, 
 * runs initialization sequences. If it receives an EEXIST catch error, hooks into 
 * the existing channel and enters the active state tracking verification safety loop.
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

    // Safety synchronization block: wait for the creator's initialization to finish
    return (sem_wait_ready(*sem_id));
}

/**
 * @brief Claims exclusive access to the critical section (Mutex down / P operation).
 * Decrements the internal semaphore structure counter resource token by -1. 
 * Blocks process execution context if the counter token value is currently 0.
 */
void    sem_lock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = -1; // Request token / decrease counter
    op.sem_flg = SEM_UNDO;  // Auto-cleanup tracking flag activated
    semop(sem_id, &op, 1);
}

/**
 * @brief Releases exclusive access to the critical section (Mutex up / V operation).
 * Increments the internal counter token value by +1, waking up any blocking peer.
 */
void    sem_unlock(int sem_id) {
    struct sembuf   op;

    op.sem_num = 0;
    op.sem_op = 1;  // Return token / increase counter
    op.sem_flg = SEM_UNDO;  // Auto-cleanup tracking flag activated
    semop(sem_id, &op, 1);
}
