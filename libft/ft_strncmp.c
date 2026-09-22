#include "libft.h"

/**
 * @brief Compares the two chains.
 *
 * @param s1 First string.
 * @param s2 Second string.
 * @param n Number of characters to be compared.
 * @return (int) The difference.
 */
int ft_strncmp(const char *s1, const char *s2, size_t n) {
	if (!n)
		return (0);
	while (*s1 && *s2 && --n && *s1 == *s2) {
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}
