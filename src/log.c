#include "display.h"
#include <stdarg.h>
#include <stdio.h>

void    log_push(t_logs *logs, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(logs->lines[logs->head], LOG_WIDTH, fmt, ap);
    va_end(ap);
    logs->head = (logs->head + 1) % LOG_COUNT;
    if (logs->count < LOG_COUNT)
        logs->count++;
}
