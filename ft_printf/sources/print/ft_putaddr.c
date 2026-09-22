#include "ft_printf.h"

int ft_putaddr(t_flag flag, unsigned long long addr) {
	if (!addr)
		return (ft_putstr(flag, "(nil)"));
	flag.hashtag = true;
	return (ft_puthexa(flag, addr, LOWER_HEXA));
}
