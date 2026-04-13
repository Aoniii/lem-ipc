/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 17:11:53 by snourry           #+#    #+#             */
/*   Updated: 2022/04/08 20:30:53 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_size(const char *s, size_t maxlen)
{
	size_t	i;

	i = 0;
	while (s[i] && i < maxlen)
		i++;
	return (i);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*new;
	size_t	i;

	i = -1;
	if (start > ft_strlen(s))
		return (ft_calloc(1, sizeof(char)));
	new = ft_calloc((ft_size(&s[start], len) + 1), sizeof(char));
	if (!new)
		return (0);
	while (++i, s[start + i] && i < len)
		new[i] = s[start + i];
	return (new);
}
