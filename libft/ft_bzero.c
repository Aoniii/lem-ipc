/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 00:49:26 by snourry           #+#    #+#             */
/*   Updated: 2022/03/29 00:49:26 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Write zero bytes in a byte block.
 * 
 * @param s The pointed memory area.
 * @param n The number of bytes replace.
 * @return (void*) The destination.
 */
void	*ft_bzero(void *s, size_t n)
{
	char	*dest;

	dest = s;
	while (n--)
		*dest++ = 0;
	return (dest);
}
