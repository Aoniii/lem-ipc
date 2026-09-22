#include "libft.h"

/**
 * @brief Writes the character to the given file descriptor.
 *
 * @param c The character to be written.
 * @param fd The file descriptor to write.
 */
void ft_putchar_fd(char c, int fd) {
	write(fd, &c, 1);
}
