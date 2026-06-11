#include "lem-ipc.h"
#include <signal.h>
#include <stddef.h>

// Global atomic flag to safely track asynchronous termination requests across processes
volatile sig_atomic_t   g_stop = 0;

/**
 * @brief Centralized signal interceptor callback routine.
 * * Sets the global stop flag to 1 when a monitored signal is caught.
 * Casts the input signal integer to void to prevent compiler unused-parameter warnings.
 */
static void handle_signal(int sig) {
    (void)sig;
    g_stop = 1; // Atomic state transition assignment
}

/**
 * @brief Configures and arms POSIX signal interception handlers for the process.
 * * Uses `sigaction` instead of the older `signal` function to guarantee 
 * reliable, non-destructive behavior across different UNIX platforms.
 * Registers hooks for both SIGINT (Ctrl+C) and SIGTERM (generic kill).
 */
void    setup_signals(void) {
    struct sigaction    sa;

    // Set the callback handler function execution point
    sa.sa_handler = handle_signal;

    // Initialize the internal signal blocking mask to empty
    sigemptyset(&sa.sa_mask);

    // Explicitly zero out flags to enforce standard blocking system-call behavior
    sa.sa_flags = 0;

    // Bind interception structures to the OS kernel listener
    sigaction(SIGINT, &sa, NULL);   // Catch terminal interrupt command (Ctrl+C)
    sigaction(SIGTERM, &sa, NULL);  // Catch termination environment requests
}
