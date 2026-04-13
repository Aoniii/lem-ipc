/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 02:39:01 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 02:39:01 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Returns a pointer to the first occurrence of the character.
 * 
 * @param s The string source.
 * @param c the character searches.
 * @return (char*) A pointer to the corresponding character.
 */
char	*ft_strchr(const char *s, int c)
{
	while ((char) c != *s)
	{
		if (!*s)
			return (0);
		s++;
	}
	return ((char *) s);
}
