#include "ft_printf.h"

int ft_printf(const char *format, ...) {
	va_list args;
	int		size;

	va_start(args, format);
	size = exec(args, (char *)format);
	va_end(args);
	return (size);
}
