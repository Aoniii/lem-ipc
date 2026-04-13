/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 17:34:30 by snourry           #+#    #+#             */
/*   Updated: 2022/03/29 17:34:30 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Copy a memory area.
 * 
 * @param dest The destination of the copy.
 * @param src The source of the copy.
 * @param n The number of bytes paste.
 * @return (void*) The destination.
 */
void	*ft_memmove(void *dest, const void *src, size_t n)
{
	char		*d;
	const char	*s;

	if (!dest && !src)
		return (dest);
	d = dest;
	s = src;
	if (d < s)
		return (ft_memcpy(dest, src, n));
	d += n - 1;
	s += n - 1;
	while (n--)
		*d-- = *s--;
	return (dest);
}
