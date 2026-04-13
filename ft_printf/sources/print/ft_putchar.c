/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:44:39 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:44:39 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putchar(t_flag flag, char c)
{
	int		size;

	size = 0;
	if (flag.minus)
		write(1, &c, 1);
	while (size < flag.padding - 1)
	{
		write(1, " ", 1);
		size++;
	}
	if (!flag.minus)
		write(1, &c, 1);
	size++;
	return (size);
}
