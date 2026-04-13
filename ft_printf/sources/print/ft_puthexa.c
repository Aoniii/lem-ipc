/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_puthexa.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:46:34 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:46:34 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static char	*apply_symbol(t_flag flag, char *out, char *base, long nbr)
{
	int	i;

	if (!flag.hashtag || nbr == 0)
		return (out);
	i = 0;
	while (out[i] == ' ')
		i++;
	if (i < 2)
		out = ft_realloc(out, 2 - i, 0, ' ');
	i = 0;
	while (out[i] == ' ')
		i++;
	out[i - 1] = base[10] + 23;
	out[i - 2] = '0';
	return (out);
}

int	ft_puthexa(t_flag flag, unsigned long long nbr, char *base)
{
	char	*out;
	int		size;

	out = ft_itoa_base(nbr, base);
	if (!out)
		return (0);
	if (flag.precision > (int) ft_strlen(out))
		out = ft_realloc(out, flag.precision - ft_strlen(out), 0, '0');
	if (nbr == 0 && flag.precision == 0)
		out = apply_zero_patch(out);
	if (flag.padding > (int) ft_strlen(out))
		out = apply_padding(flag, out);
	out = apply_symbol(flag, out, base, nbr);
	if (flag.minus)
		apply_minus(flag, out);
	size = ft_strlen(out);
	write(1, out, size);
	free(out);
	out = NULL;
	return (size);
}
