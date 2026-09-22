#include "libft.h"

/**
 * @brief Allocates the memory needed for an array.
 *
 * @param nmemb Number of the elements.
 * @param size size of the element.
 * @return (void*) A pointer to the allocated memory.
 */
void    *ft_calloc(size_t nmemb, size_t size) {
	void *ptr;

	ptr = malloc(nmemb * size);
	if (!ptr)
		return (0);
	ft_bzero(ptr, nmemb * size);
	return (ptr);
}
