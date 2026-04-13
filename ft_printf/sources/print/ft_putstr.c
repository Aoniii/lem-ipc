/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:45:04 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:45:04 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putstr(t_flag flag, char *s)
{
	int		size;
	int		strsize;

	size = 0;
	if (!s && flag.precision < 6 && flag.precision >= 0)
		return (0);
	if (!s)
		s = "(null)";
	strsize = ft_strlen(s);
	if (strsize > flag.precision && flag.precision >= 0)
		strsize = flag.precision;
	if (flag.minus)
		write(1, s, strsize);
	while (size < flag.padding - strsize)
	{
		write(1, " ", 1);
		size++;
	}
	if (!flag.minus)
		write(1, s, strsize);
	size += strsize;
	return (size);
}
