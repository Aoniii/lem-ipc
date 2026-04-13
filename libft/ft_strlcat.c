/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 02:24:52 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 02:24:52 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Concatenate the string pointed.
 * 
 * @param dest The destination of the concatenate.
 * @param src The source of the concatenate.
 * @param size The size of the destination.
 * @return (size_t) The lenght.
 */
size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (dest[i] && i < size)
		i++;
	if (i < size)
	{
		while (src[j] && i + 1 < size)
			dest[i++] = src[j++];
		dest[i] = 0;
	}
	while (src[j++])
		i++;
	return (i);
}
