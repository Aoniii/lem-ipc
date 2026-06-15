#include "display.h"
#include "replay.h"
#include "lem-ipc.h"
#include "print.h"
#include <ncurses.h>
#include <stdbool.h>
#include <unistd.h>

/**
 * @brief Applies an event to the current board (move forward).
 * JOIN places the team, MOVE shifts it, QUIT clears it.
 */
static int apply_event(t_replay *replay, t_event *ev) {
	unsigned char   *board = replay->board;
	unsigned int    w = replay->map_size;

    if (ev->event == EV_JOIN) {
        // check spawn tile is empty
        if (board[ev->y * w + ev->x] != TILE_EMPTY)
            return (-1);

		board[ev->y * w + ev->x] = ev->team;
    } else if (ev->event == EV_QUIT) {
        // check current tile is the right team
        if (board[ev->y * w + ev->x] != ev->team)
            return (-1);

		board[ev->y * w + ev->x] = TILE_EMPTY;
    } else if (ev->event == EV_MOVE) {
        // check current tile is the right team
        if (board[ev->y * w + ev->x] != ev->team)
            return (-1);
        // boundary guard check on target tile
        if (ev->x + ev->dx < 0 || ev->x + ev->dx >= (int)w || ev->y + ev->dy < 0 || ev->y + ev->dy >= (int)w)
            return (-1);
        // check target tile is empty
        if (board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] != TILE_EMPTY)
            return (-1);

		board[ev->y * w + ev->x] = TILE_EMPTY;
		board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] = ev->team;
	}

    return (0);
}

/**
 * @brief Reverts an event on the current board (move backward).
 * Undoes exactly what apply_event did.
 */
static int  undo_event(t_replay *replay, t_event *ev) {
	unsigned char   *board = replay->board;
	unsigned int    w = replay->map_size;

    if (ev->event == EV_JOIN) {
        // check current tile is the right team
        if (board[ev->y * w + ev->x] != ev->team)
            return (-1);

		board[ev->y * w + ev->x] = TILE_EMPTY;
    } else if (ev->event == EV_QUIT) {
        // check old tile is empty
        if (board[ev->y * w + ev->x] != TILE_EMPTY)
            return (-1);

		board[ev->y * w + ev->x] = ev->team;
    } else if (ev->event == EV_MOVE) {
        // check old tile is empty
        if (board[ev->y * w + ev->x] != TILE_EMPTY)
            return (-1);
        // boundary guard check on current tile
        if (ev->x + ev->dx < 0 || ev->x + ev->dx >= (int)w || ev->y + ev->dy < 0 || ev->y + ev->dy >= (int)w)
            return (-1);
        // check current tile is the right team
        if (board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] != ev->team)
            return (-1);

		board[(ev->y + ev->dy) * w + (ev->x + ev->dx)] = TILE_EMPTY;
		board[ev->y * w + ev->x] = ev->team;
	}
    return (0);
}

static t_event  *replay_last(t_replay *replay) {
	t_event *ev = replay->head;

	if (!ev)
		return (NULL);
	while (ev->next)
		ev = ev->next;
    return (ev);
}

// skips to the next event
static int  step_forward(t_replay *replay) {
	if (!replay->current)
		return (0);
	if (apply_event(replay, replay->current) == -1)
        return (-1);
	replay->ms_saved = replay->current->ms;
	replay->current = replay->current->next;
	return (1);
}

// moves back one event
static int  step_backward(t_replay *replay) {
	t_event *ev;

    // the event to cancel is the one immediately before the current one
	if (replay->current)
		ev = replay->current->prev;
	else
		ev = replay_last(replay);   // if current == NULL (end), take the last one

	if (!ev)
		return (0);                // already from the start

	if (undo_event(replay, ev) == -1)
        return (-1);
	replay->current = ev;
	replay->ms_saved = ev->ms;
	return (1);
}

int  replay_play(t_replay *replay) {
    long    last_tick;
	int     ch;
    int     b;

	if (display_init_replay(replay) == -1)
		return (-1);

	replay->playing = false;
	last_tick = now_ms();

    b = 0;
	while (!g_stop) {
		ch = getch();
		if (ch == 'q' || ch == 'Q') //quit
			break ;

		if (ch == ' ') {    // play/pause
            replay->playing = !replay->playing;
        } else if (ch == KEY_RIGHT && !replay->playing) {
            b = step_forward(replay);
        } else if (ch == KEY_LEFT && !replay->playing) {
            b = step_backward(replay);
        }

        if (b == -1)
            break ;
		
        if (replay->playing) {
			long now = now_ms();
			replay->ms_saved += (now - last_tick);
			last_tick = now;

			// apply all events where ms <= ms_saved
			while (replay->current && replay->current->ms <= replay->ms_saved) {
                b = step_forward(replay);
                if (b == -1)
                    break ;
            }

			if (!replay->current)
                replay->playing = false;
		} else {
            last_tick = now_ms();    // paused, no accumulation
		}

		display_render_replay(replay);
		usleep(1000000 / FPS);
	}

	display_destroy();
    if (b == -1)
        ft_printf("lemipc: error: illegal move\n");
    return (0);
}
