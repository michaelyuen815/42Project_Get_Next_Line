/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chyuen <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/07 14:41:32 by chyuen            #+#    #+#             */
/*   Updated: 2019/05/14 13:59:00 by chyuen           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

/*
**gnl_curlst -- sub function of get_next_line
**for controlling static linked list variable (t_list) after main process
**
**Step 1 (line 33 - 39): revised the string value of variable (content)
**			a. if no previous value, content remain unchanged (default value)
**			b. if a string without new line '\n' exists, content become NULL
**				(line 35)
**			c. if no string after new line '\n', content become NULL (line 36)
**			b. otherwise, content become string after new line '\n' (line 36)
**Step 2 (line 40 - 50): delete related node if the content is NULL
*/

static void		gnl_aflst(t_list **t_lst, t_list **t_cur)
{
	t_list	*t_tmp;
	char	*s_curfd;

	if ((*t_cur)->content)
	{
		if ((s_curfd = ft_strchr((*t_cur)->content, '\n')))
			s_curfd = (*(s_curfd + 1) ? ft_strdup(s_curfd + 1) : 0);
		free((*t_cur)->content);
		(*t_cur)->content = s_curfd;
	}
	t_tmp = *t_lst;
	while (t_tmp->next)
	{
		if (!(t_tmp->next->content))
		{
			t_tmp->next = t_tmp->next->next;
			free(*t_cur);
		}
		else
			t_tmp = t_tmp->next;
	}
}

/*
**gnl_curlst -- sub function of get_next_line
**for controlling static linked list variable (t_list)
**
**Step 1 (line 69, 70): create a new node with file discriptor (fd)
**		if linked list is not created.
**Step 2 (line 71 - 77):
**		find the corresponding node with samefile descriptor (fd)
**		if no related node is found, create a new one at the end
**Step 3: return corresponding node for processing
*/

static t_list	*gnl_curlst(t_list **t_lst, int fd)
{
	t_list	*t_curfd;

	if (!*t_lst)
		*t_lst = ft_lstnew(NULL, 0, fd);
	t_curfd = *t_lst;
	while (t_curfd->fd != fd)
	{
		if (!t_curfd->next)
			t_curfd->next = ft_lstnew(NULL, 0, fd);
		t_curfd = t_curfd->next;
	}
	return (t_curfd);
}

/*
**gnl_join -- sub function of get_next_line for :
** 1. combine strings in s_buff and content of node (s_ret) to s_ret
** 2. return string if a new complete string line is found
**
** Step 1 (line 109): extract string in s_buff based on size
** Step 2 (line 109 - 113): combine extracted string and s_ret to s_ret
** Step 3 (line 114, 115): check last read() is valid (size must larger than 0)
**			if last read() is not valid and no string is found, return (0)
** Step 4 (line 116 - 121): check whether there is complete new line found.
**			a. if no new line '\n' found and current read() return is same as
** 				BUFF_SIZE, the string of current line is not yet completed,
**				return (0) to continue read(). (line 116, 117)
**			b. if no new line '\n' found and current read() return is less than
**				BUFF_SIZE, last read() is EOF so return whole string (s_ret)
**				(line 116, 117)
**			c. if a new line '\n' found and no string found before 1st '\n',
**				return an empty string (line 118, 119)
**			b. if a new line '\n' found and string found before 1st '\n',
**				return string before 1st '\n' (line 121)
*/

static char		*gnl_join(char **s_ret, char **s_buff, int size)
{
	char *ch_chk;
	char *tmp_buf;
	char *tmp_ret;

	tmp_buf = ((size > 0) ? ft_strsub(*s_buff, 0, size) : 0);
	tmp_ret = ft_strjoin(*s_ret, tmp_buf);
	ft_strdel(&tmp_buf);
	ft_strdel(s_ret);
	*s_ret = tmp_ret;
	if (size == -1 || (!size && !tmp_ret))
		return (0);
	if (!(ch_chk = ft_strchr(tmp_ret, '\n')))
		return (size ? 0 : ft_strdup(tmp_ret));
	else if (ch_chk == tmp_ret)
		return (ft_strnew(0));
	else
		return (ft_strsub(tmp_ret, 0, (size_t)(ch_chk - tmp_ret)));
}

/*
**Main function of Get_Next_Line
**
**Step 1 (line 147, 148): Return -1 if not enough memory or
**			invalid input (fd < 0 and line is NULL)
**Step 2 (line 149): run a first read()
**Step 3 (lien 152): initalize t_cur if 1st read is valid (using gnl_curlst)
**Step 4 (line 153 - 155): Loop read() until read() is EOF or
**			there is a complete new line found (using gnl_join)
**Step 5 (line 156): revise t_list (using gnl_aflst)
**Step 4 (line 159): Return indicator:
**			a. -1 when error (return -1) during running read()
**			b. 0 when no new string found (*line is NULL)
**			c. 1 when a new string found (*line is not NULL)
*/

int				get_next_line(const int fd, char **line)
{
	int				size;
	char			*s_buff;
	t_list			*t_cur;
	static t_list	*t_lst;

	if (fd < 0 || !line || !(s_buff = ft_strnew(BUFF_SIZE)))
		return (-1);
	size = read(fd, s_buff, BUFF_SIZE);
	if (size >= 0)
	{
		t_cur = gnl_curlst(&t_lst, fd);
		while (!(*line = gnl_join((char**)&(t_cur->content), &s_buff, size)) \
			&& size > 0)
			size = read(fd, s_buff, BUFF_SIZE);
		gnl_aflst(&t_lst, &t_cur);
	}
	ft_strdel(&s_buff);
	return ((size < 0) ? -1 : (*line != 0));
}
