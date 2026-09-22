// **************************************************************************
//
//                                                        :::      ::::::::
//   get_next_line_utils_bonus.c                        :+:      :+:    :+:
//                                                    +:+ +:+         +:+
//   By: snourry <marvin@42.fr>                     +#+  +:+       +#+
//                                                +#+#+#+#+#+   +#+
//   Created: 2022/04/06 09:34:03 by snourry           #+#    #+#
//   Updated: 2022/04/15 12:11:26 by snourry          ###   ########.fr
//
// **************************************************************************

#include <stdlib.h>
#include "get_next_line_bonus.h"
#include "libft.h"

/**
 * @brief Releases the pending bytes of a fd and clears its state.
 */
void gnl_reset(t_gnl *stash) {
	free(stash->buf);
	stash->buf = NULL;
	stash->len = 0;
	stash->cap = 0;
}

/**
 * @brief Grows the stash so it can hold at least need bytes.
 *
 * The capacity doubles instead of matching the request exactly, so appending
 * stays amortized O(1) instead of copying the whole stash on every read.
 */
static int gnl_grow(t_gnl *stash, size_t need) {
	char	*grown;
	size_t	cap;

	if (need <= stash->cap)
		return (0);
	cap = stash->cap;
	if (cap == 0)
		cap = BUFFER_SIZE;
	while (cap < need)
		cap *= 2;
	grown = malloc(cap);
	if (!grown)
		return (-1);
	if (stash->len)
		ft_memcpy(grown, stash->buf, stash->len);
	free(stash->buf);
	stash->buf = grown;
	stash->cap = cap;
	return (0);
}

/**
 * @brief Appends n bytes to the stash. Returns -1 on allocation failure.
 */
int gnl_append(t_gnl *stash, const char *data, size_t n) {
	if (gnl_grow(stash, stash->len + n) == -1)
		return (-1);
	ft_memcpy(stash->buf + stash->len, data, n);
	stash->len += n;
	return (0);
}

/**
 * @brief Drops the first n bytes of the stash.
 *
 * The buffer is given back as soon as nothing is pending, so a fd read up to
 * EOF leaves no allocation behind.
 */
void gnl_consume(t_gnl *stash, size_t n) {
	if (n >= stash->len) {
		gnl_reset(stash);
		return;
	}
	ft_memmove(stash->buf, stash->buf + n, stash->len - n);
	stash->len -= n;
}

/**
 * @brief Length of the first pending line, newline included.
 *
 * Returns the whole stash when it holds no newline (last line of a file that
 * does not end with one), and 0 when the stash is empty.
 */
size_t gnl_line_len(const t_gnl *stash) {
	size_t i;

	i = 0;
	while (i < stash->len && stash->buf[i] != '\n')
		i++;
	if (i < stash->len)
		i++;
	return (i);
}
