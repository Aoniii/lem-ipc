#include <sys/time.h>
#include <stdlib.h>

/**
 * @brief Returns the current time in milliseconds.
 * Used to compute each event's offset from the start of the game.
 */
long    now_ms(void) {
	struct timeval  tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
