#include "libft.h"

/**
 * @brief Checks if we have an ascii character.
 *
 * @param c The character.
 * @return (boolean)
 */
int ft_isascii(int c) {
	return (c >= 0 && c <= 127);
}
