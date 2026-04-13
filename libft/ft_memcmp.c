/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 03:49:44 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 03:49:44 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Compares the two memory area.
 * 
 * @param s1 First memory area.
 * @param s2 Second memory area.
 * @param n Number of characters to be compared.
 * @return (int) The difference.
 */
int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char	*str1;
	unsigned char	*str2;
	size_t			i;

	str1 = (unsigned char *) s1;
	str2 = (unsigned char *) s2;
	i = -1;
	while (++i < n)
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
	return (0);
}
