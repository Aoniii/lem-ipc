/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:47:21 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:47:21 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static char	*apply_symbol(t_flag flag, char *out, long nbr)
{
	int	i;

	if (!(nbr < 0 || flag.space || flag.plus))
		return (out);
	if ((out[0] != ' ' && out[0] != '0') || !out[1] || \
	flag.precision == (int) ft_strlen(out))
		out = ft_realloc(out, 1, 0, ' ');
	i = 0;
	while (out[i] == ' ')
		i++;
	if (i > 0)
		i--;
	if (nbr < 0)
		out[i] = '-';
	else if (flag.plus)
		out[i] = '+';
	else if (flag.space)
		out[i] = ' ';
	return (out);
}

int	ft_putnbr(t_flag flag, long nbr)
{
	char	*out;
	int		size;

	out = ft_itoa(nbr);
	if (!out)
		return (0);
	if (flag.precision > (int) ft_strlen(out))
		out = ft_realloc(out, flag.precision - ft_strlen(out), 0, '0');
	if (nbr == 0 && flag.precision == 0)
		out = apply_zero_patch(out);
	if (flag.padding > (int) ft_strlen(out))
		out = apply_padding(flag, out);
	out = apply_symbol(flag, out, nbr);
	if (flag.minus)
		apply_minus(flag, out);
	size = ft_strlen(out);
	write(1, out, size);
	free(out);
	out = NULL;
	return (size);
}
