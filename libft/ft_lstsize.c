/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstsize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 11:51:46 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 11:51:46 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Counts the number of items in the list.
 * 
 * @param lst The beginning of the list.
 * @return (int) Size of the list.
 */
int	ft_lstsize(t_list *lst)
{
	int	i;

	i = -1;
	while (++i, lst)
		lst = lst->next;
	return (i);
}
