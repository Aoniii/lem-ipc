#include "libft.h"

/**
 * @brief Allocates and returns a new element. The member variable is
 * initialized with the value of the parameter.
 *
 * @param content The content of the new element.
 * @return (t_list*) The new element
 */
t_list  *ft_lstnew(void *content) {
	t_list *list;

	list = malloc(sizeof(t_list));
	if (!list)
		return (0);
	list->content = content;
	list->next = 0;
	return (list);
}
