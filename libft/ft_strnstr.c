/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 03:55:46 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 03:55:46 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Find the first substring in a length-limited string.
 * 
 * @param s1 The string to be searched.
 * @param s2 The string to search for.
 * @param len The maximum number of characters to search.
 * @return (char*) 
 */
char	*ft_strnstr(const char *s1, const char *s2, size_t len)
{
	size_t	i;
	size_t	j;

	i = -1;
	if (!*s2)
		return ((char *) s1);
	while (s1[++i])
	{
		j = -1;
		while (s2[++j] && s2[j] == s1[i + j] && i + j < len)
			if (!s2[j + 1])
				return ((char *) s1 + i);
	}
	return (0);
}
