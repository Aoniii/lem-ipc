#include "libft.h"

/**
 * @brief Fill a memory area with a given byte.
 *
 * @param s The pointed memory area.
 * @param c The byte to fill.
 * @param n The number of bytes fill.
 * @return (void*) The destination.
 */
void    *ft_memset(void *s, int c, size_t n) {
	unsigned char *ptr;

	ptr = s;
	while (n-- > 0)
		*ptr++ = c;
	return (s);
}
