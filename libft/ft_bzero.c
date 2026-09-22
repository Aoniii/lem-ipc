#include "libft.h"

/**
 * @brief Write zero bytes in a byte block.
 *
 * @param s The pointed memory area.
 * @param n The number of bytes replace.
 * @return (void*) The destination.
 */
void    *ft_bzero(void *s, size_t n) {
	char *dest;

	dest = s;
	while (n--)
		*dest++ = 0;
	return (dest);
}
