#ifndef REPLAY_H
# define REPLAY_H

# include <stdbool.h>

# define FILE_EXTENSION ".log"

typedef struct s_data   t_data;
typedef struct s_pos    t_pos;

typedef enum    e_event_type {
    EV_JOIN,
    EV_QUIT,
    EV_MOVE
}               t_event_type;

typedef struct      s_event {
    t_event_type    event;
    long            ms;     // time elapsed since the start
    unsigned int    id;     // unique ID
    unsigned char   team;
    int             x;      // position
    int             y;
    int             dx;     // direction (MOVE ONLY)
    int             dy;
    struct s_event  *next;
    struct s_event  *prev;
}                   t_event;

typedef struct      s_replay {
    t_event         *head;
    t_event         *current;
    unsigned char   *initial_board;
    unsigned char   *board;
    unsigned int    map_size;
    long            ms_saved;
    bool            playing;
}                   t_replay;

int     create_replay_file(t_data *data);
int     replay_open(t_data *data, bool write_header);
void    replay_event(t_data *data, const char *name, t_pos pos);
void    replay_join(t_data *data, t_pos pos);
void    replay_quit(t_data *data, t_pos pos);
void    replay_move(t_data *data, t_pos old, int dx, int dy);
int     replay_start(char *filename);

#endif
