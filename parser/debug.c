#include "parser.h"
#include <stdbool.h>

int ft_printf(const char *format, ...);

void    debug(char **args, const t_option *options) {
    ft_printf("--- OPTIONS ---\n");
    while (options) {
        if (options->flags == 0 && !options->short_opt && !options->long_opt)
            break;
        if (options->flags & OPT_CATEGORY) {
            options++;
            continue;
        }
        /**		Display the option name					*/
        if (options->long_opt)
            ft_printf("--%-20s ", options->long_opt);
        else
            ft_printf("-%-21c ", options->short_opt);

        ft_printf("| Value: ");

        /**		Display the value based on the type		*/
        if (!options->value) {
            ft_printf("(NULL pointer)\n");
        } else if (options->flags & TYPE_BOOLEAN) {
            ft_printf("%s\n", *((bool *)options->value) ? "TRUE" : "FALSE");
        } else if (options->flags & TYPE_INT) {
            ft_printf("%d\n", *((int *)options->value));
        } else if (options->flags & TYPE_UINT) {
            ft_printf("%u\n", *((unsigned int *)options->value));
        } else if (options->flags & TYPE_DOUBLE) {
            ft_printf("%f\n", *((double *)options->value));
        } else if (options->flags & TYPE_STRING) {
            ft_printf("\"%s\"\n", *((char **)options->value) ? *((char **)options->value) : "NULL");
        } else if (options->flags & TYPE_COUNT) {
            ft_printf("%d (count)\n", *((int *)options->value));
        } else if (options->flags & TYPE_CALLBACK) {
            ft_printf("(callback function)\n");
        } else {
            ft_printf("unknown type\n");
        }
        options++;
    }

    ft_printf("\n--- POSITIONAL ARGUMENTS ---\n");
    if (!args || !args[0]) {
        ft_printf("(none)\n");
    } else {
        for (int i = 0; args[i]; i++) {
            ft_printf("Arg[%d]: %s\n", i, args[i]);
        }
    }
    ft_printf("---------------------------\n");
}
