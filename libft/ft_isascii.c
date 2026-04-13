/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 00:12:08 by snourry           #+#    #+#             */
/*   Updated: 2022/03/29 00:12:08 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Checks if we have an ascii character.
 * 
 * @param c The character.
 * @return (boolean)
 */
int	ft_isascii(int c)
{
	return (c >= 0 && c <= 127);
}
