#include "libft.h"

/**
 * @brief Writes the integer to the given file descriptor.
 *
 * @param s The integer to be written.
 * @param fd The file descriptor to write.
 */
void ft_putnbr_fd(int n, int fd) {
	long nb;

	nb = (long) n;
	if (nb < 0) {
		ft_putchar_fd('-', fd);
		nb *= -1;
	}
	if (nb >= 10)
		ft_putnbr_fd(nb / 10, fd);
	ft_putchar_fd(nb % 10 + '0', fd);
}
