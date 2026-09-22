#include <stdlib.h>
#include "get_next_line_bonus.h"

char *ft_strjoin_gnl(char *s1, char *s2, size_t limit) {
	char	*ptr;
	size_t	i;
	size_t	j;

	i = -1;
	if (!s1) {
		s1 = (char *)malloc(1 * sizeof(char));
		*s1 = 0;
	}
	s2[limit] = 0;
	ptr = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!ptr)
		return (0);
	while (s1[++i])
		ptr[i] = s1[i];
	j = 0;
	while (s2[j])
		ptr[i++] = s2[j++];
	ptr[i] = 0;
	free(s1);
	s1 = 0;
	return (ptr);
}

size_t ft_size(char *s) {
	size_t i;

	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (s[i] == '\n')
		i++;
	return (i + 1);
}
