#include "libft.h"

/**
 * @brief Copies the string pointed.
 *
 * @param dest The destination of the copy.
 * @param src The source of the copy.
 * @param size The size of the destination.
 * @return (size_t) The lenght.
 */
size_t ft_strlcpy(char *dest, const char *src, size_t size) {
	size_t	i;
	size_t	len;

	i = -1;
	len = ft_strlen(src);
	if (!size)
		return (len);
	while (++i < size - 1 && i < len)
		dest[i] = src[i];
	dest[i] = 0;
	return (len);
}
