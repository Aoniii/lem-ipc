#include "libft.h"

/**
 * @brief Checks if we have an alphanumeric character.
 *
 * @param c The character or integer.
 * @return (boolean)
 */
int ft_isalnum(int c) {
	return (ft_isalpha(c) || ft_isdigit(c));
}
