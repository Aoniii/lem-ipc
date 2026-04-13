/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <snourry@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/04 23:59:34 by snourry           #+#    #+#             */
/*   Updated: 2023/04/04 23:59:34 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>

# define LOWER_HEXA "0123456789abcdef"
# define UPPER_HEXA "0123456789ABCDEF"

typedef struct s_flag
{
	bool	space;
	bool	hashtag;
	bool	zero;
	bool	minus;
	bool	plus;
	int		precision;
	int		padding;
}			t_flag;

/**
 * @brief printf is used to print formatted string to standard output stream.
 * 
 * @return (int) the size of written string.
 */
int					ft_printf(const char *format, ...);

/**
 *	Main functions.
 */
int					exec(va_list args, char *format);
int					convert(va_list args, char **format);

/**
 *	Print functions.
 */
int					print(va_list args, char **format, t_flag flag);
int					ft_putchar(t_flag flag, char c);
int					ft_putstr(t_flag flag, char *s);
int					ft_putaddr(t_flag flag, unsigned long long addr);
int					ft_putnbr(t_flag flag, long nbr);
int					ft_puthexa(t_flag flag, unsigned long long nbr, char *base);

/**
 *	Utils functions.
 */
bool				contains(char *s, char c);
unsigned long long	ft_strlen(char *s);
char				*ft_itoa(long nb);
char				*ft_realloc(char *str, int r, int l, char c);
char				*ft_itoa_base(unsigned long long nb, char *base);
char				*apply_padding(t_flag flag, char *out);
void				apply_minus(t_flag flag, char *out);
char				*apply_zero_patch(char *out);

#endif
