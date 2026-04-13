/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 02:05:17 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 02:05:17 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Copies the string pointed.
 * 
 * @param dest The destination of the copy.
 * @param src The source of the copy.
 * @param size The size of the destination.
 * @return (size_t) The lenght.
 */
size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	len;

	i = -1;
	len = ft_strlen(src);
	if (!size)
		return (len);
	while (++i < size - 1 && i < len)
		dest[i] = src[i];
	dest[i] = 0;
	return (len);
}
