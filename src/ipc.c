#include "ipc.h"
#include "lem-ipc.h"
#include "libft.h"
#include "board.h"
#include <errno.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Creates a persistent file on disk to be used as a token generator reference.
 * * Standard System V IPC relies on `ftok`, which requires an existing pathname.
 * This function guarantees the file's existence with defined safe creation permissions.
 */
static int  create_ipc_file(void) {
    int fd = open(IPC_PATH, O_CREAT | O_RDONLY, IPC_PERMS);
    if (fd != -1)
        close(fd);
    return (fd);
}

/**
 * @brief Handles the initial generation and setup of all shared IPC primitives.
 * * Executed exclusively by the first player process entering the game.
 * Performs exclusive segment creation (`IPC_EXCL`), configures semaphores, 
 * allocates message queues, zeroes out the grid block, and initializes metadata.
 */
static int  ipc_create(t_data *data, key_t key) {
    t_shm_header    *header;
    size_t          size = sizeof(t_shm_header) + data->map_size * data->map_size;

    // Allocate Shared Memory with exclusive flags
    data->shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->shm_id == -1) {
        if (errno == EEXIST)
            return (1); // Resource already created by an alternate process
        return (-1);
    }

    // Initialize the associated semaphore array structure
    if (sem_setup(key, &data->sem_id, &data->is_first) == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        return (-1);
    }

    // Allocate the team communication message queue
    data->msg_id = msgget(key, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->msg_id == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        semctl(data->sem_id, 0, IPC_RMID);
        return (-1);
    }

    // Map the shared memory segment into the local process address workspace
    data->shm_ptr = shmat(data->shm_id, NULL, 0);
    if (data->shm_ptr == (void *)-1)
        return (-1);

    // Initialize the shared block structure memory layout to zero
    ft_memset(data->shm_ptr, 0, size);
    header = (t_shm_header *)data->shm_ptr;
    header->map_size = data->map_size;
    header->player_count = 0;
    header->running = 1;

    // Call the procedural wall generator for custom environments if requested
    if (data->walls)
        walls_generator(data);

    // Enforce an explicit hardware-level memory barrier execution block
    __sync_synchronize();

    header->ready = 1;  // Atomic toggle notification flag for peers
    data->is_first = true;
    return (0);
}

/**
 * @brief Safety polling loop that prevents race conditions during startup.
 * Prevents joining processes from reading or writing data inside the shared
 * segment until the master creator process has fully finished memory zeroing.
 */
static int  shm_wait_ready(t_shm_header *header) {
    int retry = 0;

    while (retry < SHM_MAX_RETRY) {
        if (header->ready == 1)
            return (0);
        usleep(1000);   // Back off execution context briefly to preserve CPU overhead
        retry++;
    }
    return (-1);
}

/**
 * @brief Connects subsequent joining processes to existing game instances.
 * * Locates active shared memory segments, semaphores, and message queue IDs 
 * without modifying current state profiles. Validates configuration flags.
 */
static int  ipc_attach(t_data *data, key_t key) {
    t_shm_header    *header;
    bool            creator;

    // Attach to the current structural identifiers
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

    // Block process execution context until the initialization barrier clears
    header = (t_shm_header *)data->shm_ptr;
    if (shm_wait_ready(header) == -1)
        return (-1);

    // Import authoritative board configuration details from the master controller
    data->map_size = header->map_size;
    data->is_first = false;
    return (0);
}

/**
 * @brief General routing manager responsible for initializing the system context.
 * Guarantees the token file layout on disk, triggers initialization logic, and
 * transparently switches to secondary attachment tracking if a game is already running.
 */
int ipc_init(t_data *data) {
    key_t   key;

    if (create_ipc_file() == -1)
        return (-1);
    key = ftok(IPC_PATH, IPC_PROJ);
    if (key == -1)
        return (-1);

    // Attempt to create the match environment as the master process
    int ret = ipc_create(data, key);
    if (ret == 1)   // EEXIST catch trigger: segment already exists, route to attachment instead
        return (ipc_attach(data, key));

    return (ret);
}

/**
 * @brief Handles detaching and conditional destruction of IPC channels.
 * * Decrements player counters safely using semaphores. If the leaving agent is
 * detected as the final remaining entity on the board, deletes the structural
 * definitions from the OS Kernel entirely and removes the file from disk.
 */
void    ipc_cleanup(t_data *data) {
    t_shm_header    *header;
    bool            last;

    if (!data->shm_ptr || data->shm_ptr == (void *)-1)
        return;

    header = (t_shm_header *)data->shm_ptr;
    last = false;

    // Thread-safe update of the shared global player tracking register
    sem_lock(data->sem_id);
    if (!data->spectator && header->player_count > 0) header->player_count--;
    if (header->player_count == 0) {
        header->running = 0;
        last = true;    // Flag identifying that this process must perform final garbage collection
    }
    sem_unlock(data->sem_id);

    // Detach shared space from current virtual mapping context
    shmdt(data->shm_ptr);

    // Core Kernel garbage collector cleanup execution block
    if (last) {
        shmctl(data->shm_id, IPC_RMID, NULL);   // Wipe Shared Memory
        semctl(data->sem_id, 0, IPC_RMID);      // Wipe Semaphore Array
        msgctl(data->msg_id, IPC_RMID, NULL);   // Wipe Message Queue
        unlink(IPC_PATH);                       // Wipe identification link file
    }
}
