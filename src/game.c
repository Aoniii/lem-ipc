#include "ai.h"
#include "display.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "libft.h"
#include "player.h"
#include "print.h"
#include "board.h"
#include "game.h"
#include "ipc.h"
#include "replay.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Main entry point to initialize, run, and clean up a game session.
 * * Sets up IPCs, initializes the ncurses graphics if required, handles team
 * placement logic for players, starts the game loop, and ensures safe memory
 * and IPC teardown upon exit or crash.
 */
int game_start(t_data *data) {
    t_shm_header    *header;
    t_pos           pos = {0};
    bool            show_display;
    int             ret;
    int             nb[1];

    show_display = (data->human || data->spectator);

    // 1. Initialize IPC mechanisms (Shared memory, Semaphores, Message queues)
    if (ipc_init(data) != 0) {
        ft_printf("lemipc: error: failed to initialize IPC\n");
        return (1);
    }

    // 2. Connect player to the replay file
    if (!data->spectator) {
        data->replay_fd = replay_open(data, data->is_first);
        if (data->replay_fd == -1) {
            ft_printf("lemipc: error: failed to open replay file\n");
            return (1);
        }
    }

    // 3. Initialize display framework if running as human or spectator
    if (show_display) {
        if (display_init(data) != 0) {
            ipc_cleanup(data);
            ft_printf("lemipc: error: display init failed (terminal too small?)\n");
            return (1);
        }
    }

    // 4. Connect player to the grid (Spectators skip this step)
    if (!data->spectator) {
        if (player_place(data, &pos) == -1) {
            if (show_display) display_destroy();
            ipc_cleanup(data);
            ft_printf("lemipc: error: board is full\n");
            return (1);
        }
        player_join(data, pos);
    }

    // 5. Fire up the core loop execution state
    data->is_alive = true;
    ret = game_loop(data, show_display, &pos);

    // 6. Clean up allocations and context states upon loop break, and show statistics
    if (show_display) display_destroy();
    if (data->spectator) {
        sem_lock(data->sem_id);
        header = (t_shm_header *)data->shm_ptr;
        nb[0] = 0;
        if (!header->running && header->winner_team != 0)
            ft_printf("lemipc: winning team: %d\n", header->winner_team);
        else
            ft_printf("lemipc: The game is still in progress\n");
        ft_printf("lemipc: total turns: %d\n", header->event_id + 1);
        ft_printf("lemipc: the team with most kills: %d (%d)\n", team_with_most_kills(header->kill, nb), nb[0]);
        ft_printf("lemipc: game duration: %ld ms\n", now_ms() - header->start_ms);
        sem_unlock(data->sem_id);
    }
    ipc_cleanup(data);
    return (ret);
}

/**
 * @brief Checks if the player is dead, and updates global flags if game over.
 * Must be executed while holding the semaphore lock.
 */
static bool check_death(t_data *data, t_pos *pos, t_shm_header *header) {
	if (!is_circled(data, pos))
		return (false);

    // Player is dead: clear tile, update local status, and check for match end
	board_set_empty(data, *pos);
	data->is_alive = false;
	if (is_game_over(data))
		header->running = false;    // Notify all processes that the match is over
	return (true);
}

/**
 * @brief Processes WASD keystrokes into 2D directional displacement vectors.
 */
static void human_move(t_data *data, t_pos *pos, int ch) {
	if (ch == 'w' || ch == 'W')
		player_move(data, pos, 0, -1);
	else if (ch == 'a' || ch == 'A')
		player_move(data, pos, -1, 0);
	else if (ch == 's' || ch == 'S')
		player_move(data, pos, 0, 1);
	else if (ch == 'd' || ch == 'D')
		player_move(data, pos, 1, 0);
}

/**
 * @brief Plays one turn for this process (movement or death) under the lock.
 */
static bool play_turn(t_data *data, t_pos *pos, int ch, int *frame) {
	t_shm_header    *header;
	bool            quit;

    header = (t_shm_header *)data->shm_ptr;
	quit = false;

	sem_lock(data->sem_id); // Lock resource state before assessing conditions
	if (data->human) {
        // Human Mode: Read user character inputs
		if (ft_strchr("wWaAsSdD", ch) != 0) {
			if (check_death(data, pos, header))
				quit = true;
			else
				human_move(data, pos, ch);
		}
	} else {
        // AI Mode: Execute movement ticks based on standard FPS throttle
		if (*frame >= FPS) {
			if (check_death(data, pos, header))
				quit = true;
			else
				ai_move(data, pos);
			*frame = 0; // Reset frame threshold tracker
		}
		(*frame)++;
	}
	sem_unlock(data->sem_id);   // Unlock state safely
	return (quit);
}

/**
 * @brief Main game loop: snapshot, play a turn, render, repeat.
 */
int game_loop(t_data *data, bool show_display, t_pos *pos) {
    t_shm_header    *header;
    unsigned int    size;
    unsigned char   *snapshot;
    int             frame;
    int             ch;

    header = (t_shm_header *)data->shm_ptr;
    size = data->map_size * data->map_size;
    snapshot = NULL;
    frame = 0;

    // Allocate memory for local double-buffering if UI rendering is active
    if (show_display) {
        snapshot = malloc(size);
        if (!snapshot) {
            ft_printf("lemipc: error: memory allocation failed\n");
            return (1);
        }
    }

    // Main execution loop run condition
    while (data->is_alive && !g_stop) {
        ch = (show_display) ? getch() : ERR;

		sem_lock(data->sem_id);
        // Break early if another process declared the game is over
		if (!header->running) {
			sem_unlock(data->sem_id);
			break ;
		}

        // Take a safe local memory snapshot of the board to reduce lock contention
		if (show_display)
			ft_memcpy(snapshot, board_get(data), size);
		sem_unlock(data->sem_id);

        // Execute entity movement rules (Human or AI strategy)
		if (!data->spectator && play_turn(data, pos, ch, &frame))
			break ;

        // Render from the local snapshot (no lock held)
		if (show_display) {
			display_render(data, snapshot, pos);
			if (ch == 'q' || ch == 'Q')
				break ;
		}

        // Frame rate limiter control throttle (usleep)
        usleep(1000000 / FPS);
    }

    // --- CLEANUP EXIT SEQUENCE ---
    // If player leaves or gets killed, remove them cleanly from the system
    if (!data->spectator) {
        if (data->is_alive) {
            sem_lock(data->sem_id);
            board_set_empty(data, *pos);    // Clear my tile
            sem_unlock(data->sem_id);
        }
        player_quit(data, *pos);  // Log the exit (player_count is decremented in ipc_cleanup)
    }

    if (show_display)
        free(snapshot);
    return (0);
}

/**
 * @brief Initializes the shared game content (first player only).
 * Generates walls if requested, records the start time, and creates the
 * replay file. Called before ready = 1 so joiners see a complete board.
 */
int game_shm_init(t_data *data) {
	if (data->walls)
		walls_generator(data);
	if (create_replay_file(data) == -1)
		return (-1);
	return (0);
}
