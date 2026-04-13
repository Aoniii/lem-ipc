/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/10 11:43:11 by snourry           #+#    #+#             */
/*   Updated: 2023/05/10 11:43:11 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

char	*ft_realloc(char *str, int r, int l, char c)
{
	char	*new;
	int		i;
	int		j;

	j = ft_strlen(str);
	new = malloc(sizeof(char) * (j + r + l + 1));
	if (!new)
		return (NULL);
	i = 0;
	while (i < r)
		new[i++] = c;
	while (i - r < j)
	{
		new[i] = str[i - r];
		i++;
	}
	while (i - r - j < l)
		new[i++] = ' ';
	new[i] = 0;
	free(str);
	str = NULL;
	return (new);
}
