#include "ipc.h"
#include "game.h"
#include "lem-ipc.h"
#include "libft.h"
#include "board.h"
#include "replay.h"
#include <errno.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Creates the token file required by ftok().
 * ftok() needs an existing pathname to generate the IPC key, so we
 * guarantee the file exists before calling it.
 */
static int  create_ipc_file(void) {
    int fd = open(IPC_PATH, O_CREAT | O_RDONLY, IPC_PERMS);
    if (fd != -1)
        close(fd);
    return (fd);
}

/**
 * @brief Creates and initializes all IPC resources (first player only).
 * Exclusively creates the SHM, semaphore and message queue, zeroes the
 * board, writes the header metadata, then sets ready = 1 last so joiners
 * only read once everything is initialized.
 */
static int  ipc_create(t_data *data, key_t key) {
    t_shm_header    *header;
    size_t          size = sizeof(t_shm_header) + data->map_size * data->map_size;

    // Create the SHM segment (fails if it already exists)
    data->shm_id = shmget(key, size, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->shm_id == -1) {
        if (errno == EEXIST)
            return (1); // already created by another process
        return (-1);
    }

    // Set up the semaphore
    if (sem_setup(key, &data->sem_id, &data->is_first) == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        return (-1);
    }

    // Create the message queue
    data->msg_id = msgget(key, IPC_CREAT | IPC_EXCL | IPC_PERMS);
    if (data->msg_id == -1) {
        shmctl(data->shm_id, IPC_RMID, NULL);
        semctl(data->sem_id, 0, IPC_RMID);
        return (-1);
    }

    // Attach the SHM to our address space
    data->shm_ptr = shmat(data->shm_id, NULL, 0);
    if (data->shm_ptr == (void *)-1)
        return (-1);

    // Initialize the shared block structure memory layout to zero
    ft_memset(data->shm_ptr, 0, size);
    header = (t_shm_header *)data->shm_ptr;
    header->map_size = data->map_size;
    header->player_count = 0;
    header->running = 1;

    // Initialize walls and replay
    if (game_shm_init(data) == -1) {
        shmdt(data->shm_ptr);
        shmctl(data->shm_id, IPC_RMID, NULL);
        semctl(data->sem_id, 0, IPC_RMID);
        msgctl(data->msg_id, IPC_RMID, NULL);
        return (-1);
    }

    // Memory barrier: ensure all writes above are visible before ready = 1
    __sync_synchronize();

    header->ready = 1;  // signal joiners that init is done
    data->is_first = true;
    return (0);
}

/**
 * @brief Waits until the creator has finished initializing the SHM.
 * Joining processes spin on the ready flag before reading map_size,
 * so they never read uninitialized data.
 */
static int  shm_wait_ready(t_shm_header *header) {
    int retry = 0;

    while (retry < SHM_MAX_RETRY) {
        if (header->ready == 1)
            return (0);
        usleep(1000);   // brief sleep to avoid busy-spinning
        retry++;
    }
    return (-1);
}

/**
 * @brief Attaches a joining process to existing IPC resources.
 * Looks up the SHM, semaphore and message queue without modifying them,
 * then reads the board configuration from the header.
 */
static int  ipc_attach(t_data *data, key_t key) {
    t_shm_header    *header;
    bool            creator;

    // Look up the existing SHM id
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

    // Wait until the creator finished initializing
    header = (t_shm_header *)data->shm_ptr;
    if (shm_wait_ready(header) == -1)
        return (-1);

    // Read the map size set by the creator
    data->map_size = header->map_size;
    data->is_first = false;
    return (0);
}

/**
 * @brief Sets up the IPC layer for this process.
 * Ensures the token file exists, tries to create the resources as the
 * first player, and falls back to attaching if a game already exists.
 */
int ipc_init(t_data *data) {
    key_t   key;

    if (create_ipc_file() == -1)
        return (-1);
    key = ftok(IPC_PATH, IPC_PROJ);
    if (key == -1)
        return (-1);

    // Try to create as the first player
    int ret = ipc_create(data, key);
    if (ret == 1)   // already exists -> attach instead
        return (ipc_attach(data, key));

    return (ret);
}

/**
 * @brief Detaches from the IPC, and destroys it if this is the last player.
 * Decrements the shared player count under the semaphore. The last one
 * out removes the SHM, semaphore, message queue and token file.
 */
void    ipc_cleanup(t_data *data) {
    t_shm_header    *header;
    bool            last;

    if (!data->shm_ptr || data->shm_ptr == (void *)-1)
        return;

    header = (t_shm_header *)data->shm_ptr;
    last = false;

    // Decrement the shared player count
    sem_lock(data->sem_id);
    if (!data->spectator && header->player_count > 0) header->player_count--;
    if (header->player_count == 0) {
        header->running = 0;
        last = true;    // I'm the last one, I clean up
    }
    sem_unlock(data->sem_id);

    // Detach from the SHM
    shmdt(data->shm_ptr);

    if (last) {
        shmctl(data->shm_id, IPC_RMID, NULL);   // Remove the SHM
        semctl(data->sem_id, 0, IPC_RMID);      // Wipe Semaphore Array
        msgctl(data->msg_id, IPC_RMID, NULL);   // Wipe Message Queue
        unlink(IPC_PATH);                       // Wipe identification link file
    }
}
