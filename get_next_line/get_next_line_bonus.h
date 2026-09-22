#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stddef.h>
# include "libft.h"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE	42
# endif

char	*get_next_line(int fd);
char	*ft_strjoin_gnl(char *s1, char *s2, size_t limit);
size_t	ft_size(char *s);

#endif
