/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstdelone.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 12:09:50 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 12:09:50 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Free the memory of the element passed as an argument using
 * the function and then with free.
 * 
 * @param lst The element to free.
 * @param del The function to delete the content of the element.
 */
void	ft_lstdelone(t_list *lst, void (*del) (void*))
{
	if (lst && del)
	{
		del(lst->content);
		free(lst);
	}
}
