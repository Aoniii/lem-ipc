/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 00:16:49 by snourry           #+#    #+#             */
/*   Updated: 2022/03/29 00:16:49 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Checks if it is a printable character.
 * 
 * @param c The character.
 * @return (boolean)
 */
int	ft_isprint(int c)
{
	return (c >= 32 && c <= 126);
}
