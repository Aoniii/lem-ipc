#include "lem-ipc.h"
#include "libft.h"
#include "validation.h"
#include <stdlib.h>

t_validate  validate_args(t_data *data, char **args) {
    t_validate  validate;

    validate = validate_mode_conflicts(data);
    if (validate != V_SUCCESS)
        return (validate);

    if (!data->spectator && !data->replay) {
        validate = validate_team(args);
        if (validate != V_SUCCESS)
            return (validate);
    }

    validate = validate_ai_level(data);
    if (validate != V_SUCCESS)
        return (validate);

    validate = validate_map_size(data);
    if (validate != V_SUCCESS)
        return (validate);

    validate = validate_replay_file(data, args);
    if (validate != V_SUCCESS)
        return (validate);

    return (V_SUCCESS);
}

t_validate  validate_team(char **args) {
    if (args == NULL || args[0] == NULL)
        return (V_MISSING_TEAM);

    char    *value = args[0];
    char    *endptr = NULL;
	strtoul(value, &endptr, 10);

	if (*endptr != '\0' || endptr == value)
		return (V_INVALID_TEAM);

    return (V_SUCCESS);
}

t_validate  validate_ai_level(t_data *data) {
    if (data->ai < MIN_AI_LEVEL || data->ai > MAX_AI_LEVEL)
        return (V_INVALID_AI_LEVEL);
    return (V_SUCCESS);
}

t_validate  validate_map_size(t_data *data) {
    if (data->map_size < MIN_MAP_SIZE || data->map_size > MAX_MAP_SIZE)
        return (V_INVALID_MAP_SIZE);
    return (V_SUCCESS);
}

t_validate  validate_mode_conflicts(t_data *data) {
    if (data->spectator && data->replay)
        return (V_CONFLICT_SPECTATOR_REPLAY);
    return (V_SUCCESS);
}

t_validate  validate_replay_file(t_data *data, char **args) {
    if (!data->replay)
        return (V_SUCCESS);

    if (args == NULL || args[0] == NULL)
        return (V_REPLAY_FILE_MISSING);

    const char  *file = args[0];
    char        *ptr = ft_strnstr(file, ".log", ft_strlen(file));
    if (!ptr || ptr[4] != '\0')
        return (V_REPLAY_FILE_INVALID_EXT);

    if (access(file, R_OK) != 0)
        return (V_REPLAY_FILE_UNREADABLE);

    return (V_SUCCESS);
}

static const char   *validate_msg[] = {
    [V_SUCCESS]                     = "success",
    [V_MISSING_TEAM]                = "team number is required",
    [V_INVALID_TEAM]                = "team number must be greater than 0",
    [V_INVALID_AI_LEVEL]            = "AI level must be between " STR(MIN_AI_LEVEL) " and " STR(MAX_AI_LEVEL),
    [V_INVALID_MAP_SIZE]            = "map size must be between " STR(MIN_MAP_SIZE) " and " STR(MAX_MAP_SIZE),
    [V_CONFLICT_SPECTATOR_REPLAY]   = "--spectator and --replay are mutually exclusive",
    [V_REPLAY_FILE_MISSING]         = "replay file not found",
    [V_REPLAY_FILE_INVALID_EXT]     = "replay file must have .log extension",
    [V_REPLAY_FILE_UNREADABLE]      = "replay file is not readable",
    [V_UNEXPECTED_ARGS]             = "unexpected extra arguments",
};

const char  *validate_str(t_validate err) {
    return (validate_msg[err]);
}

