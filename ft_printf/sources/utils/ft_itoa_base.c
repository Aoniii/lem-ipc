/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/10 13:37:54 by snourry           #+#    #+#             */
/*   Updated: 2023/05/10 13:37:54 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_size(unsigned long long n, char *base)
{
	int	i;
	int	j;

	i = 1;
	j = ft_strlen(base);
	while (n > (unsigned long long)(j - 1))
	{
		n /= j;
		i++;
	}
	return (i);
}

static void	ft_convert(char *ptr, int index, unsigned long long n, char *base)
{
	int	j;

	j = ft_strlen(base);
	if (n > (unsigned long long)(j - 1))
		ft_convert(ptr, (index - 1), (n / j), base);
	ptr[index] = base[n % j];
}

char	*ft_itoa_base(unsigned long long nb, char *base)
{
	char	*ptr;
	int		size;

	size = ft_size(nb, base);
	ptr = malloc(sizeof(char) * (size + 1));
	if (!ptr)
		return (0);
	ft_convert(ptr, size - 1, nb, base);
	ptr[size] = 0;
	return (ptr);
}
