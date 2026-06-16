# lem-ipc

A multi-process battle arena where every player is a separate process. Players
fight on a shared 2D board; teams clash until only one remains. There is **no
`fork`** — each player is an independent invocation of the same binary, and the
only thing they share is a System V IPC namespace.

This is a 42 systems-programming project built around the three System V
IPC primitives: **shared memory** (the board), a **semaphore** (mutual
exclusion), and a **message queue** (team coordination).

## Game rules

- The board is a square grid. Each non-empty tile holds a team number.
- A player **dies** when at least two enemies **of the same team** are adjacent
  to it (the eight surrounding tiles, diagonals included).
- The **last team standing wins**.
- With the `--walls` bonus, a player can also be trapped and killed by
  obstacles, adding an anti-stalemate pressure to the match.

The first process to start creates the shared resources and the board; the last
one to leave destroys them.

## Build

```sh
make
```

Requirements: `gcc`, and the ncurses wide-character development headers
(`libncurses-dev` on Debian/Ubuntu). The build is warning-clean under
`-Wall -Wextra -Werror`.

Other targets: `make clean`, `make fclean`, `make re`.

## Usage

```sh
./lemipc <team> [options]
```

`<team>` is a positive integer (1 to 16) identifying the player's team. It is
**required** for normal players, and **omitted** for spectators and replay
playback.

### Options

| Option | Description |
|--------|-------------|
| `--map-size <n>` | Board size, first player only (min 5, max 25, default 10). |
| `--ai <level>` | AI level: `1` random, `2` chase, `3` coordinated (default 3). |
| `--human` | Control the player manually with the keyboard. |
| `--spectator` | Watch the game read-only; no team required. |
| `--walls` | Generate obstacles on the map, first player only. |
| `--replay <file>` | Replay a recorded game from a log file. |
| `--verbose` | Print each player's actions to stdout. |
| `--help` | Show the help list. |

Notes:

- `--map-size` and `--walls` only take effect for the **first** player, since
  they shape the board everyone else joins.
- `--spectator` and `--replay` cannot be combined.
- A human or spectator opens an ncurses display; AI players run without one.

### Controls

**Human player:** `W` `A` `S` `D` to move, `Q` to quit.

**Spectator:** `Q` to quit.

**Replay:** `SPACE` play/pause, `←` / `→` step backward/forward (while paused),
`Q` to quit.

## Examples

A quick match with walls, several AI players, and a spectator. Launch the
first player (the creator) first so it wins the race to set up the board:

```sh
# terminal 1 — creator sets the board, then more players join
./lemipc 1 --walls --map-size 15 &
sleep 0.3
./lemipc 1 & ./lemipc 1 &
./lemipc 2 & ./lemipc 2 & ./lemipc 2 &

# watch the match
./lemipc --spectator
```

Play manually against the AI:

```sh
./lemipc 1 &        # an AI teammate or opponent
./lemipc 2 --human  # you
```

Replay a recorded game:

```sh
./lemipc --replay replay/game-20260615-193012.log
```

## Verbose mode and the display

Verbose output is written to **stdout**, one line per action, prefixed with the
team and PID so concurrent processes can be told apart:

```
[team 1 | pid 12345] joined at (3, 7)
[team 2 | pid 12346] moved (3,7) -> (4,7)
[team 1 | pid 12345] eliminated
```

The ncurses display (`--human`, `--spectator`) draws directly on the terminal,
so **verbose output and a display in the same terminal will interfere** with
each other. To use both, redirect stdout to a file — the display stays in the
terminal while the logs go to the file:

```sh
./lemipc 1 --human --verbose > game.log
```

You can then follow the logs live from another terminal with `tail -f game.log`.
Alternatively, run verbose processes and the spectator in separate terminals.

## Replay system

Every game is recorded automatically to `replay/game-<date>-<time>.log`. The
file stores the initial board (size and wall layout) followed by a timestamped,
ordered stream of `JOIN`, `MOVE`, and `QUIT` events. Replay mode reads the file
back, rebuilds the board step by step, and lets you scrub through the match
forward and backward. Malformed or corrupted files are rejected at parse time
rather than crashing the player.

## Architecture

- **Shared memory** holds a header (map size, player count, timestamps,
  stats, run flags, a log ring buffer) followed by the raw board.
- **Semaphore** guards every board read/write and stat update, so the many
  processes never step on each other. It uses the robust `sem_otime`
  initialization pattern to close the create/attach race.
- **Message queue** carries per-team target coordinates, with the team number
  used as the message type to give each team its own virtual channel. This
  drives the level-3 coordinated AI.
- **AI** comes in three levels: random walk, nearest-enemy chase, and
  multi-source BFS coordination shared across a team.
- **Signals** (`SIGINT`, `SIGTERM`) set a global stop flag so every process —
  game or replay — tears down its display and releases its resources cleanly
  instead of leaking IPC objects.

If a run is interrupted hard and leaves IPC objects behind, you can inspect and
clear them with `ipcs` and `ipcrm -a`, and remove the lock file at
`/tmp/lemipc`.

## Project layout

```
include/        public headers
src/            core: ipc, board, player, game, ai, bfs, display, signals…
src/replay/     replay recorder and player
libft/          custom libc
ft_printf/      custom printf
get_next_line/  custom line reader
parser/         CLI option parser
```
