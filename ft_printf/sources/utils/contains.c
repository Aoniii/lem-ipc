#include "ft_printf.h"

bool contains(char *s, char c) {
	while (*s) {
		if (*s == c)
			return (true);
		s++;
	}
	return (false);
}
