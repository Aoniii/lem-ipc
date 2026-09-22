#include "libft.h"

/**
 * @brief Checks if it is a printable character.
 *
 * @param c The character.
 * @return (boolean)
 */
int ft_isprint(int c) {
	return (c >= 32 && c <= 126);
}
