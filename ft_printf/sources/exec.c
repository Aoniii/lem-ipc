#include "ft_printf.h"

int exec(va_list args, char *format) {
	int size;

	size = 0;
	while (*format) {
		if (*format == '%') {
			size += convert(args, &format);
			continue;
		}
		write(1, format, 1);
		size++;
		format++;
	}
	return (size);
}
