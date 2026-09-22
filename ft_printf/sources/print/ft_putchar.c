#include "ft_printf.h"

int ft_putchar(t_flag flag, char c) {
	int size;

	size = 0;
	if (flag.minus)
		write(1, &c, 1);
	while (size < flag.padding - 1) {
		write(1, " ", 1);
		size++;
	}
	if (!flag.minus)
		write(1, &c, 1);
	size++;
	return (size);
}
