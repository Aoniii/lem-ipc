/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 12:26:22 by snourry           #+#    #+#             */
/*   Updated: 2023/04/06 12:26:22 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	exec(va_list args, char *format)
{
	int	size;

	size = 0;
	while (*format)
	{
		if (*format == '%')
		{
			size += convert(args, &format);
			continue ;
		}
		write(1, format, 1);
		size++;
		format++;
	}
	return (size);
}
