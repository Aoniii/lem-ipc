#include "libft.h"

/**
 * @brief Adds the item to the top of the list.
 *
 * @param lst The address of the pointer to the first element of the list.
 * @param new The address of the pointer to the element to be added to the list.
 */
void ft_lstadd_front(t_list **lst, t_list *new) {
	if (*lst)
		new->next = *lst;
	*lst = new;
}
