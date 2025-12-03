/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 11:03:41 by mbatty            #+#    #+#             */
/*   Updated: 2025/12/03 12:33:01 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctx_bonus.h"

int	g_sig = 0;

void	handle_sig(int sig)
{
	g_sig = sig;
}

char	*server_strjoin(char *s1, char *s2);
char	*server_strdup(const char *s);
char	*server_strchr(char *s, char c);

/*

	[file_transfer]:[file_name]:[all bytes]:[delimiter]

*/

int	get_transfer(char *msg)
{
	char	*file_name = msg + 9;
	file_name[strchr(file_name, ':') - file_name] = 0;
	char	*content = file_name + strlen(file_name) + 1;

	int	fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fd == -1)
	{
		printf("Could not create %s\n", file_name);
		return (0);
	}
	write(fd, content, strlen(content) + 1);
	return (1);
}

void	msg_hook(t_client *client, char *msg, int64_t size, void *arg)
{
	(void)arg;(void)client;(void)size;
	if (client->receiving_file)
	{
		int	fd = open("transfer_out", O_CREAT | O_WRONLY | O_TRUNC, 0777);

		write(fd, (unsigned char*)msg, size);
	}
	else
		printf("\a%s\n", msg);
}

int	main(void)
{
	t_ctx	ctx;

	printf("Waiting for ft_shield to be run somewhere\n");
	signal(SIGINT, handle_sig);
	server_open(&ctx.server, SWORD_PORT);
	server_set_message_hook(&ctx.server, msg_hook, &ctx);
	while (g_sig == 0)
		server_update(&ctx.server);
	server_close(&ctx.server);
}
