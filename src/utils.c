#include "lem-ipc.h"
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

int team_with_most_kills(int *kills, int *nb) {
    int i;
    int team;

    i = 1;
    team = 0;
    while (i <= MAX_TEAM) {
        if (kills[i] > nb[0]) {
            team = i;
            nb[0] = kills[i];
        }
        i++;
    }
    return (team);
}
