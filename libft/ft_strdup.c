#include "libft.h"

/**
 * @brief Sends a pointer to a new string which is duplicated.
 *
 * @param s Source of the duplication.
 * @return (char*) The duplication.
 */
char    *ft_strdup(const char *s) {
	char	*cpy;
	int		i;

	cpy = ft_calloc(sizeof(char), (ft_strlen(s) + 1));
	if (cpy) {
		i = -1;
		while (s[++i])
			cpy[i] = s[i];
		cpy[i] = 0;
		return (cpy);
	}
	return (0);
}
