/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_str.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 13:29:53 by mbatty            #+#    #+#             */
/*   Updated: 2025/12/02 21:46:42 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ctx.h"

uint32_t	hash_str(const char *input)
{
	uint32_t hash = 0x811c9dc5;
	while (*input)
	{
		hash ^= (uint8_t)(*input++);
		hash *= 0x01000193;
	}
	return (hash);
}
