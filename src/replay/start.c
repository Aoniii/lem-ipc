#include "replay.h"
#include "libft.h"
#include "print.h"
#include <fcntl.h>

/**
 * @brief Frees everything allocated in the replay struct.
 * Walks the event list freeing each node, then the two boards.
 */
static void replay_free(t_replay *replay) {
	t_event *cur;
	t_event *next;

	cur = replay->head;
	while (cur) {
		next = cur->next;
		free(cur);
		cur = next;
	}

	replay->head = NULL;
	replay->current = NULL;
	free(replay->initial_board);
	free(replay->board);
	replay->initial_board = NULL;
	replay->board = NULL;
}

int replay_start(char *filename) {
    t_replay    replay;
    int         fd;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        ft_printf("lemipc: error: failed to open the replay file\n");
        return (-1);
    }

    ft_memset(&replay, 0, sizeof(t_replay));
    if (replay_parse(&replay, fd) == -1) {
        close(fd);
        replay_free(&replay);
        ft_printf("lemipc: error: failed to parse the replay file\n");
        return (-1);
    }

    close(fd);
    if (replay_play(&replay) == -1) {
        replay_free(&replay);
		ft_printf("lemipc: error: unplayable event in replay\n");
        return (-1);
    }

    replay_free(&replay);
    return (0);
}
