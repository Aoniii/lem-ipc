/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   apply.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/11 16:41:07 by snourry           #+#    #+#             */
/*   Updated: 2023/05/11 16:41:07 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

char	*apply_padding(t_flag flag, char *out)
{
	char	c;

	c = ' ';
	if (flag.zero && !flag.minus && \
	(flag.padding < flag.precision || flag.precision == -1))
		c = '0';
	out = ft_realloc(out, flag.padding - ft_strlen(out), 0, c);
	return (out);
}

void	apply_minus(t_flag flag, char *out)
{
	int	i;
	int	j;

	i = 0;
	while (out[i] == ' ')
		i++;
	if (flag.space)
		i--;
	if (i <= 0)
		return ;
	j = 0;
	while (out[i])
	{
		out[j] = out[i];
		j++;
		i++;
	}
	while (out[j])
	{
		out[j] = ' ';
		j++;
	}
}

char	*apply_zero_patch(char *out)
{
	free(out);
	out = NULL;
	out = malloc(sizeof(char) * 1);
	out[0] = 0;
	return (out);
}
