/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:02:41 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 12:02:41 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Adds the element to the end of the list.
 * 
 * @param lst The address of the pointer to the first element of the list.
 * @param new The address of the pointer to the element to be added to the list.
 */
void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*l;

	if (new)
	{
		if (!*lst)
		{
			*lst = new;
			return ;
		}
		l = ft_lstlast(*lst);
		l->next = new;
	}
}
