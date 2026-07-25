#include "ipc.h"
#include "lem-ipc.h"
#include "print.h"
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

/**
 * @brief Thread-safe message logger that pushes formatted strings into a shared ring buffer.
 * * Formats inputs similarly to `printf` using variadic arguments, stores the result at
 * the current sliding head index, and atomically wraps the boundary using modulo arithmetic.
 * Must be called while holding the semaphore lock if multi-process collision is possible.
 */
void    log_push(t_data *data, const char *fmt, ...) {
    t_shm_header    *header;
    va_list         ap;

    header = (t_shm_header *)data->shm_ptr;

    // 1. Process and format the variadic arguments into the shared logs matrix row
    va_start(ap, fmt);
    vsnprintf(header->logs[header->log_head], LOG_WIDTH, fmt, ap);
    va_end(ap);

    // 2. Advance the write pointer and wrap it around using circular buffer math
    header->log_head = (header->log_head + 1) % LOG_COUNT;

    // 3. Increment total valid active logs count until reaching the max allocation threshold
    if (header->log_count < LOG_COUNT)
        header->log_count++;
}

/**
 * @brief Prints a player's action to stdout when verbose mode is on.
 * Prefixed with team and PID so concurrent AI processes sharing a
 * terminal can be told apart. No-op when verbose is disabled.
 */
void    verbose_log(t_data *data, const char *fmt, ...) {
	char    buf[256];
	va_list ap;

	if (!data->verbose)
		return ;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	ft_printf("[team %d | pid %d] %s\n", data->team, getpid(), buf);
	fflush(stdout);
}
