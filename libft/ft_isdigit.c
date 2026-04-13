/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/29 00:05:19 by snourry           #+#    #+#             */
/*   Updated: 2022/03/29 00:05:19 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Checks if we have a number.
 * 
 * @param c The integer.
 * @return (boolean)
 */
int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}
