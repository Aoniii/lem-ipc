/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 01:20:16 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 01:20:16 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Applies the 'f' function to each character in the string passed as
 * an argument to create a new string.
 * 
 * @param s The string to iterate on.
 * @param f The function to be applied to each character.
 * @return (char*) The string resulting from the successive applications of 'f'.
 */
char	*ft_strmapi(char const *s, char (*f) (unsigned int, char))
{
	char	*str;
	int		i;

	str = ft_calloc((ft_strlen(s) + 1), sizeof(char));
	if (!str)
		return (0);
	i = -1;
	while (s[++i])
		str[i] = f(i, s[i]);
	str[i] = 0;
	return (str);
}
