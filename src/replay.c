#include "ipc.h"
#include "lem-ipc.h"
#include "replay.h"
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/**
 * @brief Initializes a new game replay file.
 * * This function retrieves the current working directory, ensures a "/replay" 
 * folder exists, and generates a timestamped filename (e.g., game-YYYYMMDD-HHMMSS.log).
 * The absolute path is saved into the shared memory for later use.
 */
int create_replay_file(t_data *data) {
    t_shm_header    *header;
    char            cwd[PATH_MAX];
    char            dir[PATH_MAX + 64];
    int             fd;
    time_t          now;
    struct tm       *t;

    header = (t_shm_header *)data->shm_ptr;

    // 1. Retrieve the absolute directory
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return (-1);

    // 2. Create the replay folder (absolute path)
    snprintf(dir, sizeof(dir), "%s/replay", cwd);
    mkdir(dir, 0755);

    // 3. Generate the filename based on the date and time
    now = time(NULL);
    t = localtime(&now);
    snprintf(header->replay_path, sizeof(header->replay_path),
        "%s/replay/game-%04d%02d%02d-%02d%02d%02d.log",
        cwd, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    // 4. Create the file
    fd = open(header->replay_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return (-1);
    close(fd);
    return (0);
}
