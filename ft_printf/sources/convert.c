#include "ft_printf.h"

static t_flag flag_init(void) {
	t_flag flag;

	flag.space = false;
	flag.hashtag = false;
	flag.zero = false;
	flag.minus = false;
	flag.plus = false;
	flag.precision = -1;
	flag.padding = 0;
	return (flag);
}

int convert(va_list args, char **format) {
	t_flag	flag;
	int		size;

	format[0]++;
	if (!format[0])
		return (0);
	flag = flag_init();
	size = print(args, format, flag);
	return (size);
}
