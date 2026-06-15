#include "display.h"
#include "ipc.h"
#include "lem-ipc.h"
#include "player.h"
#include "board.h"
#include "replay.h"
#include <stdlib.h>

/**
 * @brief Thread-safely places a new player on a random available empty tile.
 * * To guarantee perfect uniform distribution without endless guessing loops, 
 * it counts the current empty tiles, picks a random target index within that range, 
 * and scans the board to claim exactly that $N$-th empty slot.
 */
int player_place(t_data *data, t_pos *pos) {
    unsigned char   *board;
    unsigned int    count;
    unsigned int    rdm;
    unsigned int    i;

    sem_lock(data->sem_id);

    // 1. Safety check: gather how many tiles are currently free
    count = board_count(data, TILE_EMPTY);
    if (count == 0) {
        sem_unlock(data->sem_id);
        return (-1);
    }

    board = board_get(data);
    rdm = rand() % count;   // Target the N-th empty tile safely
    i = 0;

    // 2. Iterate through the grid to locate the chosen empty slot
    while (i < data->map_size * data->map_size) {
        if (board[i] == TILE_EMPTY) {
            if (rdm == 0) break;
            rdm--;
        }
        i++;
    }

    // 3. Register the player's team on the board and convert 1D index to 2D coordinates
    board[i] = data->team;
    pos->x = i % data->map_size;
    pos->y = i / data->map_size;

    sem_unlock(data->sem_id);
    return (0);
}

/**
 * @brief Registers a player's entry into the active match session.
 * Increments the global shared player counter and posts a joining log broadcast.
 */
void    player_join(t_data *data, t_pos pos) {
    sem_lock(data->sem_id);
    log_push(data, "[+] Team %d joined at (%d, %d)", data->team, pos.x, pos.y);
    verbose_log(data, "joined at (%d, %d)", pos.x, pos.y);
    ((t_shm_header *)data->shm_ptr)->player_count++;
    replay_join(data, pos);
    sem_unlock(data->sem_id);
}

/**
 * @brief Registers a player's deletion footprint from the active game session.
 * Logs the exit status event. Note: global player_count decrement is safely 
 * handled inside the centralized ipc_cleanup sequence.
 */
void    player_quit(t_data *data, t_pos pos) {
    sem_lock(data->sem_id);
    log_push(data, "[-] Team %d left", data->team);
    verbose_log(data, "left");
    replay_quit(data, pos);
    sem_unlock(data->sem_id);
}

/**
 * @brief Moves the player one tile if the target is free: clears the old tile,
 * claims the new one, logs the move.
 */
bool    player_move(t_data *data, t_pos *pos, int dx, int dy) {
    unsigned char   *board;
    int             nx = pos->x + dx;
    int             ny = pos->y + dy;

    // 1. Boundary guard check
    if (nx < 0 || nx >= (int)data->map_size || ny < 0 || ny >= (int)data->map_size)
        return (false);

    // 2. Collision guard check
    board = board_get(data);
    if (board[ny * data->map_size + nx] != TILE_EMPTY)
        return (false);

    // 3. Move: clear old tile, claim new one.
    board[pos->y * data->map_size + pos->x] = TILE_EMPTY;   // Clear old position
    board[ny * data->map_size + nx] = data->team;           // Claim new position

    log_push(data, "[>] Team %d moved from (%d, %d) to (%d, %d)", data->team, pos->x, pos->y, nx, ny);
    verbose_log(data, "moved (%d,%d) -> (%d,%d)", pos->x, pos->y, nx, ny);
    replay_move(data, *pos, dx, dy);

    // Update local tracker data
    pos->x = nx;
    pos->y = ny;
    return (true);
}
