/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 04:15:18 by snourry           #+#    #+#             */
/*   Updated: 2022/03/30 04:15:18 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Convert a string to int.
 * 
 * @param nptr The string convert.
 * @return (int) The result.
 */
int	ft_atoi(const char *nptr)
{
	int	result;
	int	sign;

	sign = 1;
	result = 0;
	while (*nptr == ' ' || (*nptr >= '\t' && *nptr <= '\r'))
		nptr++;
	if (*nptr == '+' || *nptr == '-')
		if (*nptr++ == '-')
			sign *= -1;
	nptr--;
	while (*++nptr >= '0' && *nptr <= '9')
	{
		if (result == 0)
			result = *nptr - 48;
		else
			result = result * 10 + (*nptr - 48);
	}
	return (result * sign);
}
