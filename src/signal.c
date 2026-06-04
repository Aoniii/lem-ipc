#include "lem-ipc.h"
#include <signal.h>
#include <stddef.h>

volatile sig_atomic_t   g_stop = 0;

static void handle_signal(int sig) {
    (void)sig;
    g_stop = 1;
}

void    setup_signals(void) {
    struct sigaction    sa;

    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}
