/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 11:32:52 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 11:32:52 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Allocates and returns a new element. The member variable is 
 * initialized with the value of the parameter.
 * 
 * @param content The content of the new element.
 * @return (t_list*) The new element
 */
t_list	*ft_lstnew(void *content)
{
	t_list	*list;

	list = malloc(sizeof(t_list));
	if (!list)
		return (0);
	list->content = content;
	list->next = 0;
	return (list);
}
