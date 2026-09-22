#include "libft.h"

/**
 * @brief Supprime et libère la mémoire de l’élément passé en paramètre, et de
 * tous les éléments qui suivent, à l’aide de la fonction et de free.
 *
 * @param lst The address of the pointer to an element.
 * @param del The address of the function to delete the content of an delete the
 * content of an element.
 */
void ft_lstclear(t_list **lst, void (*del) (void *)) {
	t_list *l;

	if (*lst) {
		while (*lst) {
			l = (*lst)->next;
			ft_lstdelone(*lst, del);
			*lst = l;
		}
		*lst = 0;
	}
}
