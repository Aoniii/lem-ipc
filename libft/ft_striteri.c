/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/01 01:29:04 by snourry           #+#    #+#             */
/*   Updated: 2022/04/01 01:29:04 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Applies the 'f' function to each character in the string passed
 * as argument, passing its index as the first argument.
 * 
 * @param s The string to iterate on.
 * @param f  The function to be applied to each character.
 */
void	ft_striteri(char *s, void (*f) (unsigned int, char*))
{
	int	i;

	i = -1;
	while (s[++i])
		f(i, &s[i]);
}
