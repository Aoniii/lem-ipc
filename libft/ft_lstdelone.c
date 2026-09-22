#include "libft.h"

/**
 * @brief Free the memory of the element passed as an argument using the
 * function and then with free.
 *
 * @param lst The element to free.
 * @param del The function to delete the content of the element.
 */
void ft_lstdelone(t_list *lst, void (*del) (void *)) {
	if (lst && del) {
		del(lst->content);
		free(lst);
	}
}
