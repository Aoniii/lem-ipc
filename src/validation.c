#include "lem-ipc.h"
#include "libft.h"
#include "replay.h"
#include "validation.h"
#include <stdlib.h>

/**
 * @brief Parses and validates the positional team argument.
 */
t_validate  validate_args(t_data *data, char **args) {
    t_validate  validate;

    // 1. Check for mutually exclusive runtime flags
    validate = validate_mode_conflicts(data);
    if (validate != V_SUCCESS)
        return (validate);

    // 2. Enforce team requirements (Spectators and Replay readers skip this)
    if (!data->spectator && !data->replay) {
        validate = validate_team(data, args);
        if (validate != V_SUCCESS)
            return (validate);
    }

    // 3. Verify AI capabilities match available logic modules
    validate = validate_ai_level(data);
    if (validate != V_SUCCESS)
        return (validate);

    // 4. Validate physical dimensions bounds configurations
    validate = validate_map_size(data);
    if (validate != V_SUCCESS)
        return (validate);

    // 5. Assess the tracking log integrity file if in playback mode
    validate = validate_replay_file(data, args);
    if (validate != V_SUCCESS)
        return (validate);

    return (V_SUCCESS);
}

/**
 * @brief Validates and extracts the positional Team ID integer argument.
 * * Uses `strtoul` for robust conversion, checking for empty strings, non-numeric
 * pollution, overflow vulnerabilities, and explicit authorized limits.
 */
t_validate  validate_team(t_data *data, char **args) {
    char            *value;
    char            *endptr;
	unsigned int    result;

    if (args == NULL || args[0] == NULL)
        return (V_MISSING_TEAM);

    value = args[0];
    endptr = NULL;
	result = strtoul(value, &endptr, 10);

    // Error if characters remain unparsed (*endptr) or no digits were evaluated entirely
	if (*endptr != '\0' || endptr == value)
		return (V_INVALID_TEAM);

    // Boundary check limits enforcement
    if (result < 1 || result > MAX_TEAM)
        return (V_INVALID_TEAM_RANGE);

    data->team = result;    // Save verified parameter to runtime profile
    return (V_SUCCESS);
}

/**
 * @brief Verifies that the designated AI difficulty level matches authorized rules.
 */
t_validate  validate_ai_level(t_data *data) {
    if (data->ai < MIN_AI_LEVEL || data->ai > MAX_AI_LEVEL)
        return (V_INVALID_AI_LEVEL);
    return (V_SUCCESS);
}

/**
 * @brief Checks the map size is within MIN_MAP_SIZE and MAX_MAP_SIZE.
 */
t_validate  validate_map_size(t_data *data) {
    if (data->map_size < MIN_MAP_SIZE || data->map_size > MAX_MAP_SIZE)
        return (V_INVALID_MAP_SIZE);
    return (V_SUCCESS);
}

/**
 * @brief Rejects mutually exclusive mode flags.
 */
t_validate  validate_mode_conflicts(t_data *data) {
    if (data->spectator && data->replay)
        return (V_CONFLICT_SPECTATOR_REPLAY);
    if (data->spectator && data->human)
        return (V_CONFLICT_SPECTATOR_HUMAN);
    if (data->human && data->replay)
        return (V_CONFLICT_HUMAN_REPLAY);
    return (V_SUCCESS);
}

/**
 * @brief Validates file accessibility, system permissions, and naming layout constraints.
 * Uses `access(..., R_OK)` to verify file readability before starting the replay loop.
 */
t_validate  validate_replay_file(t_data *data, char **args) {
    const char  *file = args[0];
    char        *ptr;

    if (!data->replay)
        return (V_SUCCESS);

    if (args == NULL || args[0] == NULL)
        return (V_REPLAY_FILE_MISSING);

    ptr = ft_strnstr(file, FILE_EXTENSION, ft_strlen(file));

    // Ensure the extension exists and terminates the string footprint exactly
    if (!ptr || ptr[ft_strlen(FILE_EXTENSION)] != '\0')
        return (V_REPLAY_FILE_INVALID_EXT);

    // Check the file is readable
    if (access(file, R_OK) != 0)
        return (V_REPLAY_FILE_UNREADABLE);

    return (V_SUCCESS);
}

// Error code -> message
static const char   *validate_msg[] = {
    [V_SUCCESS]                     = "success",
    [V_MISSING_TEAM]                = "team number is required",
    [V_INVALID_TEAM]                = "team number must be greater than 0",
    [V_INVALID_TEAM_RANGE]          = "team number must be between 1 and " STR(MAX_TEAM),
    [V_INVALID_AI_LEVEL]            = "AI level must be between " STR(MIN_AI_LEVEL) " and " STR(MAX_AI_LEVEL),
    [V_INVALID_MAP_SIZE]            = "map size must be between " STR(MIN_MAP_SIZE) " and " STR(MAX_MAP_SIZE),
    [V_CONFLICT_SPECTATOR_REPLAY]   = "--spectator and --replay are mutually exclusive",
    [V_CONFLICT_SPECTATOR_HUMAN]    = "--spectator and --human are mutually exclusive",
    [V_CONFLICT_HUMAN_REPLAY]       = "--human and --replay are mutually exclusive",
    [V_REPLAY_FILE_MISSING]         = "replay file not found",
    [V_REPLAY_FILE_INVALID_EXT]     = "replay file must have .log extension",
    [V_REPLAY_FILE_UNREADABLE]      = "replay file is not readable",
    [V_UNEXPECTED_ARGS]             = "unexpected extra arguments",
};

/**
 * @brief Converts internal evaluation enumeration flags into user-facing text strings.
 */
const char  *validate_str(t_validate err) {
    return (validate_msg[err]);
}

