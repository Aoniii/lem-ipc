#include "parser.h"

int ft_printf(const char *format, ...);

void	error(const char *program, t_parser_ctx *ctx) {
	switch (ctx->err) {
		case ERR_UNKNOW_OPTION:
			ft_printf("%s: unrecognized option ('%s')\n", program, ctx->token);
			break;
		case ERR_MISSING_VALUE:
			ft_printf("%s: option ('%s') requires an argument\n", program, ctx->token);
			break;
		case ERR_INVALID_FORMAT:
			ft_printf("%s: invalid value ('%s')\n", program, ctx->value);
			break;
		case ERR_OVERFLOW:
			ft_printf("%s: value overflow of option ('%s')\n", program, ctx->token);
			break;
		case ERR_MALLOC_FAILED:
			ft_printf("%s: allocation failed\n", program);
			break;
		default:
			break;
	}
}

