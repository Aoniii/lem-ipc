/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 03:41:18 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 03:41:18 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Examines the pointed memory area for the character.
 * 
 * @param s The memory area.
 * @param c The character.
 * @param n The number of search limit.
 * @return (void*) A pointer to the corresponding byte.
 */
void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*str;

	i = -1;
	str = (unsigned char *) s;
	while (++i < n)
		if (str[i] == (unsigned char) c)
			return (&str[i]);
	return (0);
}
