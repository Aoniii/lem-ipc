#include "libft.h"

/**
 * @brief Writes the string to the given file descriptor followed by a line
 * break.
 *
 * @param s The string to be written.
 * @param fd The file descriptor to write.
 */
void ft_putendl_fd(char *s, int fd) {
	write(fd, s, ft_strlen(s));
	write(fd, "\n", 1);
}
