#include "libft.h"

/**
 * @brief Checks if we have an alphabetical character.
 *
 * @param c The character.
 * @return (boolean)
 */
int ft_isalpha(int c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}
