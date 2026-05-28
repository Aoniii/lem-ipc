#include "lem-ipc.h"
#include "parser.h"
#include "validation.h"
#include "print.h"
#include <stdbool.h>

int main(int argc, char** argv) {
    t_parser_ctx    ctx;
    ctx.err = PARSER_SUCCESS;

    t_parser_info   info = {
        .program        = argv[0],
        .usage          = "",
        .description    = "",
        .footer         = ""
    };

    t_data  data = {
        .map_size = DEFAULT_MAP_SIZE,
        .ai = DEFAULT_AI_LEVEL,
        .spectator = false,
        .walls = false,
        .replay = false,
        .verbose = false
    };

    t_option    options[] = {
        {
            .short_opt  = 0,
            .long_opt   = "map-size",
            .flags      = OPT_LONG | TYPE_UINT,
            .value      = &data.map_size,
            .help       = "set map size (first player only, default: 10)"
        },
        {
            .short_opt  = 0,
            .long_opt   = "ai",
            .flags      = OPT_LONG | TYPE_UINT,
            .value      = &data.ai,
            .help       = "set AI level: 1=random, 2=chase, 3=coordinated (default: 1)"
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
        {0}
    };

    char **args = parser(argc, argv, options, MODE_PERMISSIVE, &ctx);
    if (ctx.err != PARSER_SUCCESS) {
		error(info.program, &ctx);
		cleaner(args);
		return (ctx.err == CALLBACK_EXIT ? 0 : 1);
	}

    t_validate  ret = validate_args(&data, args);
    if (ret != V_SUCCESS) {
        ft_printf("lemipc: error: %s\n", validate_str(ret));
        return (1);
    }

    cleaner(args);
    return (0);
}
