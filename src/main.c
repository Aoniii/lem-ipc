#include "lem-ipc.h"
#include "parser.h"
#include "validation.h"
#include "print.h"
#include "game.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv) {
    t_parser_ctx    ctx;
    ctx.err = PARSER_SUCCESS;

    // Help text shown by --help
    t_parser_info   info = {
        .program        = argv[0],
        .usage          = "",
        .description    = "",
        .footer         = ""
    };

    // Default configuration
    t_data  data = {
        .map_size = DEFAULT_MAP_SIZE,
        .ai = DEFAULT_AI_LEVEL,
        .spectator = false,
        .human = false,
        .walls = false,
        .replay = false,
        .verbose = false,
        .team = 0
    };

    // CLI options
    t_option    options[] = {
        {
            .short_opt  = 0,
            .long_opt   = "map-size",
            .flags      = OPT_LONG | TYPE_UINT,
            .value      = &data.map_size,
            .help       = "set map size (first player only, default: " STR(DEFAULT_MAP_SIZE) ")"
        },
        {
            .short_opt  = 0,
            .long_opt   = "ai",
            .flags      = OPT_LONG | TYPE_UINT,
            .value      = &data.ai,
            .help       = "set AI level: 1=random, 2=chase, 3=coordinated (default: " STR(DEFAULT_AI_LEVEL) ")"
        },
        {
            .short_opt  = 0,
            .long_opt   = "spectator",
            .flags      = OPT_LONG | TYPE_BOOLEAN,
            .value      = &data.spectator,
            .help       = "launch as spectator (read-only, no team required)"
        },
        {
            .short_opt  = 0,
            .long_opt   = "human",
            .flags      = OPT_LONG | TYPE_BOOLEAN,
            .value      = &data.human,
            .help       = "launch as human"
        },
        {
            .short_opt  = 0,
            .long_opt   = "walls",
            .flags      = OPT_LONG | TYPE_BOOLEAN,
            .value      = &data.walls,
            .help       = "generate obstacles on the map (first player only)"
        },
        {
            .short_opt  = 0,
            .long_opt   = "replay",
            .flags      = OPT_LONG | TYPE_BOOLEAN,
            .value      = &data.replay,
            .help       = "replay a recorded game from file"
        },
        {
            .short_opt  = 'v',
            .long_opt   = "verbose",
            .flags      = OPT_SHORT | OPT_LONG | TYPE_BOOLEAN,
            .value      = &data.verbose,
            .help       = "enable debug output"
        },
        {
			.short_opt	= 0,
			.long_opt	= "help",
			.flags		= OPT_LONG | OPT_CALLBACK_EXIT | TYPE_CALLBACK,
			.value		= (void *)&(t_callback_info){
				.fn = callback_help,
				.data = (void *)&(t_help_data){
					.info = info,
					.options = options
				}
			},
			.help		= "give this help list"
		},
        {0} // Array terminator marker requirement
    };

    // 1. Parse command-line positional flags and key-value assignments
    char **args = parser(argc, argv, options, MODE_PERMISSIVE, &ctx);
    if (ctx.err != PARSER_SUCCESS) {
		error(info.program, &ctx);
		cleaner(args);
		return (ctx.err == CALLBACK_EXIT ? 0 : 1);
	}

    // 2. Perform validation logic matching on unmapped remaining arguments (e.g., Team ID)
    t_validate  validate = validate_args(&data, args);
    if (validate != V_SUCCESS) {
        ft_printf("lemipc: error: %s\n", validate_str(validate));
        return (1);
    }

    // 3. Setup system interception hooks (SIGINT, SIGTERM, etc.) for graceful shutdown
    setup_signals();

    // 4. Seed RNG with PID + time so concurrent players differ
    srand(getpid() ^ time(NULL));

    // 5. Normal game, or replay mode
    int ret = 0;
    if (!data.replay) {
        ret = game_start(&data);
    } else {
        // TODO: add replay log handling system
    }

    // Free the parsed args
    cleaner(args);
    return (ret);
}
