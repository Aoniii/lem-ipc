#include "libft.h"

/**
 * @brief Iterates over the list and applies the function to the content of each
 * item.
 *
 * @param lst The address of the pointer to an element.
 * @param f The address of the function to be applied.
 */
void ft_lstiter(t_list *lst, void (*f) (void *)) {
	while (lst) {
		f(lst->content);
		lst = lst->next;
	}
}
