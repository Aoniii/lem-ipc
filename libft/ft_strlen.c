#include "libft.h"

/**
 * @brief Calculates the length of the string.
 *
 * @param s The string.
 * @return (int) Its length.
 */
size_t ft_strlen(const char *s) {
	size_t size;

	size = 0;
	while (s[size])
		size++;
	return (size);
}
