/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 17:03:00 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 17:03:00 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Sends a pointer to a new string which is duplicated.
 * 
 * @param s Source of the duplication.
 * @return (char*) The duplication.
 */
char	*ft_strdup(const char *s)
{
	char	*cpy;
	int		i;

	cpy = ft_calloc(sizeof(char), (ft_strlen(s) + 1));
	if (cpy)
	{
		i = -1;
		while (s[++i])
			cpy[i] = s[i];
		cpy[i] = 0;
		return (cpy);
	}
	return (0);
}
