/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 03:08:47 by snourry           #+#    #+#             */
/*   Updated: 2022/04/06 12:04:27 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Returns a pointer to the last occurrence of the character.
 * 
 * @param s The string source.
 * @param c the character searches.
 * @return (char*) A pointer to the corresponding character.
 */
char	*ft_strrchr(const char *s, int c)
{
	size_t	i;
	char	*str;

	i = -1;
	str = 0;
	while (s[++i])
		if (s[i] == (char) c)
			str = (char *) &s[i];
	if ((char)c == s[i])
		return ((char *) &s[i]);
	return (str);
}
