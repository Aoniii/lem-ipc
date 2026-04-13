/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:43:26 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:43:26 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	print(va_list args, char **format, t_flag flag)
{
	int	size;

	size = 0;
	if (**format == 'c')
		size += ft_putchar(flag, va_arg(args, int));
	else if (**format == 's')
		size += ft_putstr(flag, va_arg(args, char *));
	else if (**format == 'p')
		size += ft_putaddr(flag, va_arg(args, unsigned long long));
	else if (**format == 'd' || **format == 'i')
		size += ft_putnbr(flag, va_arg(args, int));
	else if (**format == 'u')
		size += ft_putnbr(flag, va_arg(args, unsigned int));
	else if (**format == 'x')
		size += ft_puthexa(flag, va_arg(args, unsigned int), LOWER_HEXA);
	else if (**format == 'X')
		size += ft_puthexa(flag, va_arg(args, unsigned int), UPPER_HEXA);
	else if (**format == '%')
		size += ft_putchar(flag, '%');
	format[0]++;
	return (size);
}
