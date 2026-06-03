#include "ipc.h"
#include "lem-ipc.h"
#include <stdarg.h>
#include <stdio.h>

void    log_push(t_data *data, const char *fmt, ...) {
    t_shm_header    *header = (t_shm_header *)data->shm_ptr;
    va_list         ap;

    va_start(ap, fmt);
    vsnprintf(header->logs[header->log_head], LOG_WIDTH, fmt, ap);
    va_end(ap);
    header->log_head = (header->log_head + 1) % LOG_COUNT;
    if (header->log_count < LOG_COUNT)
        header->log_count++;
}
