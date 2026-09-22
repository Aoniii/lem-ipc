#include "libft.h"

/**
 * @brief Returns a pointer to the first occurrence of the character.
 *
 * @param s The string source.
 * @param c the character searches.
 * @return (char*) A pointer to the corresponding character.
 */
char    *ft_strchr(const char *s, int c) {
	while ((char) c != *s) {
		if (!*s)
			return (0);
		s++;
	}
	return ((char *) s);
}
