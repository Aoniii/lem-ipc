/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: snourry <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/06 09:28:06 by snourry           #+#    #+#             */
/*   Updated: 2022/04/15 10:28:33 by snourry          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"
#include <unistd.h>
#include <stdlib.h>

static char	*ft_read(int fd, char *backup)
{
	char		*buffer;
	long long	i;

	buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!buffer)
		return (0);
	i = 1;
	*buffer = 0;
	while (!ft_strchr(buffer, '\n') && i)
	{
		i = read(fd, buffer, BUFFER_SIZE);
		if (i < 0)
		{
			free(buffer);
			buffer = 0;
			return (0);
		}
		buffer[i] = 0;
		backup = ft_strjoin(backup, buffer, i);
	}
	free(buffer);
	buffer = 0;
	return (backup);
}

static char	*ft_free_backup(char *backup, size_t i)
{
	char	*new;
	size_t	j;

	if (!i)
	{
		free(backup);
		backup = 0;
		return (0);
	}
	new = malloc(sizeof(char) * (ft_strlen(backup) - i + 1));
	if (!new)
		return (0);
	j = 0;
	while (backup[i])
		new[j++] = backup[i++];
	new[j] = 0;
	free(backup);
	backup = 0;
	return (new);
}

static char	*ft_get_line(char *backup)
{
	char	*line;
	size_t	i;

	if (!*backup)
		return (0);
	line = malloc(sizeof(char) * ft_size(backup));
	if (!line)
		return (0);
	i = -1;
	while (++i, backup[i] && backup[i] != '\n')
		line[i] = backup[i];
	if (backup[i] == '\n')
		line[i++] = '\n';
	line[i] = 0;
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*backup[1024];
	char		*line;

	if ((fd < 0 || fd > 1023) && BUFFER_SIZE < 1)
		return (0);
	backup[fd] = ft_read(fd, backup[fd]);
	if (!backup[fd])
		return (0);
	line = ft_get_line(backup[fd]);
	backup[fd] = ft_free_backup(backup[fd], ft_size(backup[fd]) - 1);
	return (line);
}
