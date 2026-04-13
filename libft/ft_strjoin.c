/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 22:37:55 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 22:37:55 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Allows and returns a new string, the result of the concatenation.
 * 
 * @param s1 The prefix string.
 * @param s2 The suffix string.
 * @return (char*) The new string.
 */
char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ptr;
	size_t	i;

	i = -1;
	ptr = ft_calloc((ft_strlen(s1) + ft_strlen(s2) + 1), sizeof(char));
	if (!ptr)
		return (0);
	while (s1[++i])
		ptr[i] = s1[i];
	i = ft_strlen(s1) - 1;
	while (++i, i < ft_strlen(s1) + ft_strlen(s2) && s2)
		ptr[i] = s2[i - ft_strlen(s1)];
	ptr[i] = 0;
	return (ptr);
}
