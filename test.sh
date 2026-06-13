#!/bin/bash

# === Configuration ===
TEAMS=3            # number of teams
PLAYERS_PER_TEAM=3 # players per team
AI_LEVEL=3         # AI level (1=random, 2=chase, 3=coordination)
MAP_SIZE=15        # map size
WALLS=1            # 1 = with walls, 0 = without

# === Building Creator Options ===
CREATOR_OPTS="--map-size $MAP_SIZE --ai $AI_LEVEL"
if [ "$WALLS" -eq 1 ]; then
    CREATOR_OPTS="$CREATOR_OPTS --walls"
fi

# === Clearing the IPC from a previous game (if any) ===
ipcrm -a 2>/dev/null
rm -f /tmp/lemipc 2>/dev/null

# === Launching the first player (creator) ===
echo "Launching creator: team 1 $CREATOR_OPTS"
./lemipc 1 $CREATOR_OPTS &
sleep 0.3

# === Remaining Players ===
for team in $(seq 1 $TEAMS); do
    start=1
    if [ "$team" -eq 1 ]; then
        start=2
    fi
    for player in $(seq $start $PLAYERS_PER_TEAM); do
        ./lemipc $team --ai $AI_LEVEL &
    done
done

# === Spectator ===
sleep 0.2
./lemipc --spectator
