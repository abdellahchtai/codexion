/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_dongles.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 17:10:42 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/08 18:44:06 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_top(t_dongle *dongle)
{
	if (dongle->heap_size == 0)
		return (-1);
	return (dongle->heap[0].coder_id);
}

int	dongles_ready(t_dongle *first, t_dongle *second, t_request req)
{
	long	now;

	now = get_time_on_ms(NULL);
	return (first->available && now >= first->available_at && second->available
		&& now >= second->available_at && heap_top(first) == req.coder_id
		&& heap_top(second) == req.coder_id);
}

t_request	req_push_to_heap(t_coder *coder)
{
	t_request	req;

	req.coder_id = coder->coder_id;
	if (coder->args->scheduler[0] == 'f')
	{
		pthread_mutex_lock(&coder->args->mtx_fifo);
		req.key = coder->args->fifo_order++;
		pthread_mutex_unlock(&coder->args->mtx_fifo);
		req.tie_breaker = req.key;
	}
	else
	{
		pthread_mutex_lock(&coder->mutex_last_compile);
		req.key = coder->last_compile + coder->args->time_to_burnout;
		pthread_mutex_unlock(&coder->mutex_last_compile);
		pthread_mutex_lock(&coder->args->mtx_fifo);
		req.tie_breaker = coder->args->fifo_order++;
		pthread_mutex_unlock(&coder->args->mtx_fifo);
	}
	req.id = req.tie_breaker;
	heap_push(&coder->dongles[coder->first_dg], req);
	heap_push(&coder->dongles[coder->second_dg], req);
	return (req);
}

void	use_dongles(t_coder *coder, int one_coder_flag)
{
	t_dongle	*first;
	t_dongle	*second;

	first = &coder->dongles[coder->first_dg];
	second = &coder->dongles[coder->second_dg];
	if (one_coder_flag)
	{
		pthread_mutex_lock(&first->lock);
		ft_printf_mutex(coder, "has taken a dongle", 0);
		pthread_mutex_unlock(&first->lock);
		return ;
	}
	heap_pop(first);
	heap_pop(second);
	first->available = 0;
	second->available = 0;
	lock_unlock_dongles(first, second, 0);
	ft_printf_mutex(coder, "has taken a dongle", 0);
	ft_printf_mutex(coder, "has taken a dongle", 0);
}

int	get_both_dongles(t_coder *coder)
{
	t_dongle		*first;
	t_dongle		*second;
	t_request		req;

	first = &coder->dongles[coder->first_dg];
	second = &coder->dongles[coder->second_dg];
	req = req_push_to_heap(coder);
	while (!is_burnout(coder->args))
	{
		if (coder->first_dg == coder->second_dg)
			return (use_dongles(coder, 1), 0);
		lock_unlock_dongles(first, second, 1);
		if (dongles_ready(first, second, req))
			return (use_dongles(coder, 0), 1);
		lock_unlock_dongles(first, second, 0);
		wait_dongles(first, second);
	}
	return (0);
}
