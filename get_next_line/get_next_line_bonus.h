// **************************************************************************
//
//                                                        :::      ::::::::
//   get_next_line_bonus.h                              :+:      :+:    :+:
//                                                    +:+ +:+         +:+
//   By: snourry <marvin@42.fr>                     +#+  +:+       +#+
//                                                +#+#+#+#+#+   +#+
//   Created: 2022/04/06 09:28:19 by snourry           #+#    #+#
//   Updated: 2022/04/13 19:23:43 by snourry          ###   ########.fr
//
// **************************************************************************

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stddef.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE	42
# endif

# define GNL_MAX_FD	1024

/**
 * @brief Bytes read from a fd but not returned as a line yet.
 *
 * cap is tracked so that appending is amortized O(1): growing the buffer by
 * doubling keeps the cost of reading one line linear in its length.
 */
typedef struct s_gnl {
	char	*buf;
	size_t	len;
	size_t	cap;
}	t_gnl;

char	*get_next_line(int fd);
int		gnl_append(t_gnl *stash, const char *data, size_t n);
void	gnl_consume(t_gnl *stash, size_t n);
void	gnl_reset(t_gnl *stash);
size_t	gnl_line_len(const t_gnl *stash);

#endif
