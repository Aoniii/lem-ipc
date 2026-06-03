#ifndef VALIDATION_H
# define VALIDATION_H

typedef struct s_data   t_data;

typedef enum    e_validate {
    V_SUCCESS,
    V_MISSING_TEAM,
    V_INVALID_TEAM,
    V_INVALID_TEAM_RANGE,
    V_INVALID_AI_LEVEL,
    V_INVALID_MAP_SIZE,
    V_CONFLICT_SPECTATOR_REPLAY,
    V_CONFLICT_SPECTATOR_HUMAN,
    V_CONFLICT_HUMAN_REPLAY,
    V_REPLAY_FILE_MISSING,
    V_REPLAY_FILE_INVALID_EXT,
    V_REPLAY_FILE_UNREADABLE,
    V_UNEXPECTED_ARGS
}               t_validate;

t_validate  validate_args(t_data *data, char **args);
t_validate  validate_team(t_data *data, char **args);
t_validate  validate_ai_level(t_data *data);
t_validate  validate_map_size(t_data *data);
t_validate  validate_mode_conflicts(t_data *data);
t_validate  validate_replay_file(t_data *data, char **args);
const char  *validate_str(t_validate err);

#endif
