/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 09:34:03 by snourry           #+#    #+#             */
/*   Updated: 2022/04/15 12:11:26 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <stdlib.h>

char	*ft_strchr(const char *s, int c)
{
	if (!s)
		return (0);
	while (*s)
	{
		if (*s == c)
			return ((char *) s);
		s++;
	}
	return (0);
}

char	*ft_strjoin(char *s1, char *s2, size_t limit)
{
	char	*ptr;
	size_t	i;
	size_t	j;

	i = -1;
	if (!s1)
	{
		s1 = (char *)malloc(1 * sizeof(char));
		*s1 = 0;
	}
	s2[limit] = 0;
	ptr = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!ptr)
		return (0);
	while (s1[++i])
		ptr[i] = s1[i];
	j = 0;
	while (s2[j])
		ptr[i++] = s2[j++];
	ptr[i] = 0;
	free(s1);
	s1 = 0;
	return (ptr);
}

size_t	ft_strlen(char *s)
{
	size_t	size;

	size = 0;
	while (s[size])
		size++;
	return (size);
}

size_t	ft_size(char *s)
{
	size_t	i;

	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (s[i] == '\n')
		i++;
	return (i + 1);
}
