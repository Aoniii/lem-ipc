/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putaddr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:45:45 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:45:45 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_putaddr(t_flag flag, unsigned long long addr)
{
	if (!addr)
		return (ft_putstr(flag, "(nil)"));
	flag.hashtag = true;
	return (ft_puthexa(flag, addr, LOWER_HEXA));
}
