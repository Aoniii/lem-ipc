/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 23:35:59 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 23:35:59 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_ischarset(char c, const char *set)
{
	size_t	i;

	i = -1;
	while (set[++i])
		if (c == set[i])
			return (1);
	return (0);
}

/**
 * @brief Allows and returns a copy of the string.
 * 
 * @param s1 The string to be trimmed.
 * @param set The reference set of characters to be trimmed.
 * @return (char*) The trimmed string.
 */
char	*ft_strtrim(char const *s1, char const *set)
{
	char	*ptr;
	int		i;
	int		j;
	int		end;

	i = 0;
	if (!s1 || !set)
		return (0);
	while (s1[i] && ft_ischarset(s1[i], set))
		i++;
	end = (ft_strlen(s1) - 1);
	while (end > i && ft_ischarset(s1[end], set))
		end--;
	ptr = ft_calloc((end - i + 2), sizeof(char));
	if (!ptr)
		return (0);
	j = -1;
	while (++j, i + j <= end)
		ptr[j] = s1[i + j];
	ptr[j] = 0;
	return (ptr);
}
