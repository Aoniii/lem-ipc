/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/04 21:43:42 by snourry           #+#    #+#             */
/*   Updated: 2023/05/04 21:43:42 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_size(long n)
{
	int	i;

	i = 1;
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

char	*ft_itoa(long nb)
{
	char	*ptr;
	int		size;

	if (nb < 0)
		nb = -nb;
	size = ft_size(nb);
	ptr = malloc(sizeof(char) * (size + 1));
	if (!ptr)
		return (0);
	ft_convert(ptr, size - 1, nb);
	ptr[size] = 0;
	return (ptr);
}
