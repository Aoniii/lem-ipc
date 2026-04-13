/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/08 14:39:22 by snourry           #+#    #+#             */
/*   Updated: 2023/04/08 14:39:22 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

unsigned long long	ft_strlen(char *s)
{
	unsigned long long	size;

	size = 0;
	if (!s)
		return (0);
	while (s[size])
		size++;
	return (size);
}
