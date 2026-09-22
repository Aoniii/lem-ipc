// **************************************************************************
//
//                                                        :::      ::::::::
//   get_next_line_bonus.c                              :+:      :+:    :+:
//                                                    +:+ +:+         +:+
//   By: snourry <marvin@42.fr>                     +#+  +:+       +#+
//                                                +#+#+#+#+#+   +#+
//   Created: 2022/04/06 09:28:06 by snourry           #+#    #+#
//   Updated: 2022/04/15 10:28:33 by snourry          ###   ########.fr
//
// **************************************************************************

#include <stdlib.h>
#include <unistd.h>
#include "get_next_line_bonus.h"
#include "libft.h"

/**
 * @brief Reads from fd until the stash holds a newline or the file ends.
 *
 * Only the freshly read chunk is scanned for the newline, never the whole
 * stash, so reading a long line stays linear. Returns -1 on a read or
 * allocation error.
 */
static int gnl_fill(int fd, t_gnl *stash) {
	char	buffer[BUFFER_SIZE];
	ssize_t readed;

	if (stash->len && ft_memchr(stash->buf, '\n', stash->len))
		return (0);
	readed = read(fd, buffer, BUFFER_SIZE);
	while (readed > 0) {
		if (gnl_append(stash, buffer, (size_t)readed) == -1)
			return (-1);
		if (ft_memchr(buffer, '\n', (size_t)readed))
			return (0);
		readed = read(fd, buffer, BUFFER_SIZE);
	}
	if (readed < 0)
		return (-1);
	return (0);
}

/**
 * @brief Returns the next line of fd, newline included, or NULL at EOF.
 *
 * Every exit path that returns NULL releases the fd's pending bytes, so a
 * caller that stops reading early (or hits an error) leaks nothing.
 */
char *get_next_line(int fd) {
	static t_gnl	stash[GNL_MAX_FD];
	char			*line;
	size_t			len;

	if (fd < 0 || fd >= GNL_MAX_FD || BUFFER_SIZE < 1)
		return (NULL);
	if (gnl_fill(fd, &stash[fd]) == -1) {
		gnl_reset(&stash[fd]);
		return (NULL);
	}

	len = gnl_line_len(&stash[fd]);
	if (len == 0) {
		gnl_reset(&stash[fd]);
		return (NULL);
	}

	line = malloc(len + 1);
	if (!line) {
		gnl_reset(&stash[fd]);
		return (NULL);
	}
	ft_memcpy(line, stash[fd].buf, len);
	line[len] = '\0';
	gnl_consume(&stash[fd], len);
	return (line);
}
