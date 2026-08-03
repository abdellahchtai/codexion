/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:32:22 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/03 20:24:57 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	dongle_ready(t_dongle *dongle, int coder_id)
{
	long	now;

	now = get_time_on_ms(NULL);
	return (dongle->available && dongle->available_at <= now && heap_top(dongle) == coder_id);
}

int	have_dongle(t_coder *coder, int dg_id)
{
	t_request		req;
	t_dongle		*dongle;
	struct timeval	tv;
	struct timespec	ts;

	dongle = &coder->dongles[dg_id];
	req.coder_id = coder->coder_id;
	if (coder->args->scheduler[0] == 'f')
		req.key = get_next_ticket(coder->args);
	else
	{
		pthread_mutex_lock(&coder->mutex_last_compile);
		req.key = coder->last_compile + coder->args->time_to_burnout;
		pthread_mutex_unlock(&coder->mutex_last_compile);
	}
	pthread_mutex_lock(&dongle->lock);
	heap_push(dongle, req);
	while (!dongle_ready(dongle, coder->coder_id) && !is_burnout(coder->args))
	{
		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000 + 2000000;
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
	}
	if (is_burnout(coder->args))
		return (pthread_mutex_unlock(&dongle->lock), 0);
	heap_pop(dongle);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);
	ft_printf_mutex(coder, "has taken a dongle");
	return (1);
}

void	put_dongle(t_dongle *dongle, int cooldown)
{
	long	now;
	
	now = get_time_on_ms(NULL);
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	dongle->available_at = now + cooldown;
	pthread_mutex_unlock(&dongle->lock);
	pthread_cond_broadcast(&dongle->cond);
}

void	release_dongles(t_coder *coder)
{
	int	cooldown;
	
	cooldown = coder->args->dongle_cooldown;
	put_dongle(&coder->dongles[coder->first_dg], cooldown);
	put_dongle(&coder->dongles[coder->second_dg], cooldown);
}

int	get_both_dongles(t_coder *coder)
{
	if (!have_dongle(coder, coder->first_dg))
		return (0);
	if (!have_dongle(coder, coder->second_dg))
		return(put_dongle(&coder->dongles[coder->first_dg],
				coder->args->dongle_cooldown), 0);
	return (1);
}
