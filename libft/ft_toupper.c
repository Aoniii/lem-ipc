#include "libft.h"

/**
 * @brief Convert a letter to uppercase.
 *
 * @param c The character.
 * @return (int) The ascii value of result.
 */
int ft_toupper(int c) {
	if (c >= 'a' && c <= 'z')
		c -= 32;
	return (c);
}
