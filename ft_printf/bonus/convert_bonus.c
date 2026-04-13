/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   convert_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/09 14:44:39 by snourry           #+#    #+#             */
/*   Updated: 2023/04/09 14:44:39 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	get_int(char **format)
{
	int	result;

	result = 0;
	while (**format >= '0' && **format <= '9')
	{
		result = (result * 10) + (**format - 48);
		format[0]++;
	}
	format[0]--;
	return (result);
}

static t_flag	flag_init(void)
{
	t_flag	flag;

	flag.space = false;
	flag.hashtag = false;
	flag.zero = false;
	flag.minus = false;
	flag.plus = false;
	flag.precision = -1;
	flag.padding = 0;
	return (flag);
}

static t_flag	get_flag(char **format)
{
	t_flag	flag;

	flag = flag_init();
	while (!contains("cspdiuxX%", **format))
	{
		if (**format == ' ')
			flag.space = true;
		else if (**format == '#')
			flag.hashtag = true;
		else if (**format == '0')
			flag.zero = true;
		else if (**format == '.')
		{
			format[0]++;
			flag.precision = get_int(format);
		}
		else if (**format == '-')
			flag.minus = true;
		else if (**format == '+')
			flag.plus = true;
		else if (**format > '0' && **format <= '9')
			flag.padding = get_int(format);
		format[0]++;
	}
	return (flag);
}

int	convert(va_list args, char **format)
{
	t_flag	flag;
	int		size;

	format[0]++;
	if (!format[0])
		return (0);
	flag = get_flag(format);
	size = print(args, format, flag);
	return (size);
}
