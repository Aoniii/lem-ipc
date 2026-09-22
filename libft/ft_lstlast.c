#include "libft.h"

/**
 * @brief Returns the last item in the list.
 *
 * @param lst The beginning of the list.
 * @return (t_list*) Last item in the list.
 */
t_list  *ft_lstlast(t_list *lst) {
	while (lst) {
		if (!lst->next)
			return (lst);
		lst = lst->next;
	}
	return (0);
}
