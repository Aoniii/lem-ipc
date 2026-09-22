#include "libft.h"

/**
 * @brief Returns a pointer to the last occurrence of the character.
 *
 * @param s The string source.
 * @param c the character searches.
 * @return (char*) A pointer to the corresponding character.
 */
char    *ft_strrchr(const char *s, int c) {
	size_t	i;
	char	*str;

	i = -1;
	str = 0;
	while (s[++i])
		if (s[i] == (char) c)
			str = (char *) &s[i];
	if ((char)c == s[i])
		return ((char *) &s[i]);
	return (str);
}
