/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 13:43:09 by snourry           #+#    #+#             */
/*   Updated: 2022/03/31 13:43:09 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_free_all(char **dest, size_t index)
{
	while (index > 0)
	{
		free(dest[index]);
		index--;
	}
	free(dest);
}

static size_t	ft_words(const char *s, char c)
{
	size_t	i;
	size_t	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i] && s[i] != c)
			count++;
		while (s[i] && s[i] != c)
			i++;
	}
	return (count);
}

static void	ft_write(char *ptr, char const *s, char c)
{
	size_t	i;

	i = -1;
	while (++i, s[i] != c && s[i])
		ptr[i] = s[i];
	ptr[i] = 0;
}

static void	ft_tab(char **ptr, char const *s, char c)
{
	size_t	i;
	size_t	j;
	size_t	index;

	i = 0;
	index = 0;
	while (s[i])
	{
		if (s[i] && s[i] != c)
		{
			j = 0;
			while (s[i + j] != c && s[i + j])
				j++;
			ptr[index] = ft_calloc(sizeof(char), j + 1);
			if (!ptr[index])
				return (ft_free_all(ptr, index));
			ft_write(ptr[index], (s + i), c);
			i += j;
			index++;
		}
		else
			i++;
	}
	ptr[index] = 0;
}

/**
 * @brief Allows and returns an array of strings obtained.
 * 
 * @param s The string to be cut.
 * @param c The delimiter character.
 * @return (char**) The array of new strings resulting from the split.
 */
char	**ft_split(char const *s, char c)
{
	char	**ptr;

	if (!s)
		return (0);
	ptr = ft_calloc(sizeof(char *), ft_words(s, c) + 1);
	if (!ptr)
		return (0);
	ft_tab(ptr, (char *) s, c);
	return (ptr);
}
