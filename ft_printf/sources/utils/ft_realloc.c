#include "ft_printf.h"

char *ft_realloc(char *str, int r, int l, char c) {
	char	*new;
	int		i;
	int		j;

	j = ft_strlen(str);
	new = malloc(sizeof(char) * (j + r + l + 1));
	if (!new)
		return (NULL);
	i = 0;
	while (i < r)
		new[i++] = c;
	while (i - r < j) {
		new[i] = str[i - r];
		i++;
	}
	while (i - r - j < l)
		new[i++] = ' ';
	new[i] = 0;
	free(str);
	str = NULL;
	return (new);
}
