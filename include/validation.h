#ifndef VALIDATION_H
# define VALIDATION_H

typedef struct s_data   t_data;

// Entry point — runs all validations, returns 0 on success, 1 on error
int     validate_args(t_data *data, char **args, int count);

// Parse and assign team number from args
// Must be > 0, required unless --spectator or --replay is set
int     validate_team(t_data *data, char **args);

// Ensure AI level is between 1 and MAX_AI_LEVEL
int     validate_ai_level(t_data *data);

// Ensure map size is within MIN_MAP_SIZE and MAX_MAP_SIZE
int     validate_map_size(t_data *data);

// Check for mutually exclusive flags (e.g. --spectator + --replay)
int     validate_mode_conflicts(t_data *data);

// In replay mode, check that the target file exists and is readable
int     validate_replay_file(t_data *data, char **args);

#endif
