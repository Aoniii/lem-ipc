#include "libft.h"

/**
 * @brief Iterates over the list and applies the function to the contents of
 * each item. Creates a new list resulting from the successive applications.
 *
 * @param lst The address of the pointer to an element.
 * @param f L'adresse de la fonction à appliquer.
 * @param del The address of the function to delete the content of an delete the
 * content of an element.
 * @return (t_list*) The new list.
 */
t_list  *ft_lstmap(t_list *lst, void *(*f) (void *), void (*del) (void *)) {
	t_list	*l;
	t_list	*l1;

	l1 = ft_lstnew(f(lst->content));
	if (!l1) {
		ft_lstclear(&lst, del);
		return (0);
	}
	lst = lst->next;
	l = l1;
	while (lst) {
		l1 = ft_lstnew(f(lst->content));
		if (!l1) {
			ft_lstclear(&lst, del);
			ft_lstclear(&l, del);
			break;
		}
		lst = lst->next;
		ft_lstadd_back(&l, l1);
	}
	return (l);
}
