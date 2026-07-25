# lem-ipc

*[Version française](README.fr.md)*

A multi-process battle arena where every player is a separate process. Players
fight on a shared 2D board; teams clash until only one remains. There is **no
`fork`** — each player is an independent invocation of the same binary, and the
only thing they share is a System V IPC namespace.

This is a 42 systems-programming project built around the three System V IPC
primitives: **shared memory** (the board), a **semaphore** (mutual exclusion),
and a **message queue** (team coordination).

## Game rules

- The board is a square grid. Each non-empty tile holds a team number, and a
  tile holds at most one player.
- A player **dies** when at least two enemies **of the same team** are adjacent
  to it — the eight surrounding tiles, diagonals included. Two enemies from
  *different* teams are not enough.
- Players see team numbers, not identities: you cannot tell two members of the
  same team apart.
- The **last team standing wins**.

The first process to start creates the shared resources and the board; the last
one to leave destroys them.

## Build

```sh
make
```

Requirements: `gcc` and the ncurses wide-character development headers
(`libncurses-dev` on Debian/Ubuntu). The build is warning-clean under
`-Wall -Wextra -Werror`.

The root `Makefile` builds `libft` by calling `libft/Makefile`, then compiles
the project and links against `libft.a` with `-L libft -lft`.

Other targets: `make clean`, `make fclean`, `make re`.

## Usage

```sh
./lemipc <team> [options]
```

`<team>` is an integer from 1 to 16 identifying the player's team. It is
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
| `--replay <file>` | Replay a recorded game from a `.log` file. |
| `--verbose` | Print each player's actions to stdout. |
| `--help` | Show the help list. |

Notes:

- `--map-size` and `--walls` only take effect for the **first** player, since
  they shape the board everyone else joins.
- `--human`, `--spectator` and `--replay` are mutually exclusive — no two of
  them can be combined.
- A human or spectator opens an ncurses display; AI players run without one.
- Walls are terrain: they block movement and pathfinding, and they are recorded
  in the replay file. They never kill.

### Controls

**Human player:** `W` `A` `S` `D` to move, `Q` to quit.

**Spectator:** `Q` to quit.

**Replay:** `SPACE` play/pause, `←` / `→` step backward/forward (while paused),
`Q` to quit.

## Examples

The quickest way to see a full match is the bundled script, which launches
several teams, walls and a spectator in one go:

```sh
./test.sh
```

To do it by hand, launch the first player (the creator) first so it wins the
race to set up the board:

```sh
# the creator sets the board, then more players join
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

When the spectator exits, it prints a summary of the match:

```
lemipc: winning team: 1
lemipc: total turns: 70
lemipc: the team with most kills: 1 (4)
lemipc: game duration: 10565 ms
```

## Verbose mode and the display

Verbose output is written to **stdout**, one line per action, prefixed with the
team and PID so concurrent processes can be told apart:

```
[team 1 | pid 12345] joined at (3, 7)
[team 2 | pid 12346] moved (3,7) -> (4,7)
[team 1 | pid 12345] left
```

A player logs `left` both when it is killed and when it quits on its own.

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
ordered stream of `JOIN`, `MOVE` and `QUIT` events:

```
MAP 10
BOARD 0000000000000000000000000000000000...
0 0 JOIN 1 2 0
403 1 JOIN 2 7 3
1005 2 MOVE 1 2 0 1 0
```

Replay mode reads the file back, rebuilds the board step by step, and lets you
scrub through the match forward and backward. The parser validates the header,
event ordering, ids, team numbers and move legality, so malformed or corrupted
files are rejected up front rather than crashing the player.

## Architecture

- **Shared memory** holds a header (map size, player count, timestamps, kill
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
libft/          custom libc (own Makefile)
ft_printf/      custom printf
get_next_line/  custom line reader
parser/         CLI option parser
test.sh         launches a full demo match
```
