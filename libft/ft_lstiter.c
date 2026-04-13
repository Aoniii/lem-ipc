/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:28:44 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 12:28:44 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Iterates over the list and applies the function
 * to the content of each item.
 * 
 * @param lst The address of the pointer to an element.
 * @param f The address of the function to be applied.
 */
void	ft_lstiter(t_list *lst, void (*f) (void *))
{
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}
