#include "libft.h"

/**
 * @brief Counts the number of items in the list.
 *
 * @param lst The beginning of the list.
 * @return (int) Size of the list.
 */
int ft_lstsize(t_list *lst) {
	int i;

	i = -1;
	while (++i, lst)
		lst = lst->next;
	return (i);
}
