#include "libft.h"

/**
 * @brief Convert a letter to lowercase.
 *
 * @param c The character.
 * @return (int) The ascii value of result.
 */
int ft_tolower(int c) {
	if (c >= 'A' && c <= 'Z')
		c += 32;
	return (c);
}
