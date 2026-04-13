/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/31 16:51:46 by snourry           #+#    #+#             */
/*   Updated: 2022/03/31 16:51:46 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_size(long n)
{
	int	i;

	i = 0;
	if (n < 0)
	{
		n = -n;
		i++;
	}
	while (n > 9)
	{
		n /= 10;
		i++;
	}
	return (i);
}

static void	ft_convert(char *ptr, int index, long n)
{
	char	*base;

	base = "0123456789";
	if (n > 9)
		ft_convert(ptr, (index - 1), (n / 10));
	ptr[index] = base[n % 10];
}

/**
 * @brief Allows and returns a string string representing the integer.
 * 
 * @param n The integer to be converted.
 * @return (char*) The string representing the integer.
 */
char	*ft_itoa(int n)
{
	char	*ptr;
	long	nb;
	int		size;

	nb = (long) n;
	size = ft_size(nb);
	ptr = ft_calloc(size + 2, sizeof(char));
	if (!ptr)
		return (0);
	if (nb < 0)
	{
		ptr[0] = '-';
		nb = -nb;
	}
	ft_convert(ptr, size, nb);
	ptr[size + 1] = 0;
	return (ptr);
}
