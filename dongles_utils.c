/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:32:22 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:11:02 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	dongle_ready(t_dongle *dongle, int first, int second)
{
	long	time_now;

	time_now = get_time_on_ms(NULL, 0);
	return (dongle[first].available_at <= time_now
		&& dongle[second].available_at <= time_now);
}

void	get_dongle_order(t_coder coder, int *first, int *last)
{
	int	right_dg;
	int	left_dg;

	left_dg = coder.coder_id - 1;
	if (coder.coder_id == 1)
		right_dg = coder.args->nb_of_coders - 1;
	else
		right_dg = coder.coder_id - 2;
	if (right_dg < left_dg)
	{
		*first = right_dg;
		*last = left_dg;
	}
	else
	{
		*first = left_dg;
		*last = right_dg;
	}
}

long	ft_max(long time1, long time2)
{
	if (time1 >= time2)
		return (time1);
	return (time2);
}

void	wait_for_dongles(t_coder *coder, int dg1, int dg2)
{
	long			max;
	struct timespec	timeout;

	lock_unlock_dongle(coder, dg1, dg2, 1);
	max = ft_max(coder->dongles[dg1].available_at,
			coder->dongles[dg2].available_at);
	lock_unlock_dongle(coder, dg1, dg2, 0);
	timeout.tv_sec = max / 1000;
	timeout.tv_nsec = max % 1000 * 1000000;
	pthread_mutex_lock(&(coder->args->shared->mutex_wait));
	pthread_cond_timedwait(&(coder->args->shared->cond_wait),
		&(coder->args->shared->mutex_wait), &timeout);
	pthread_mutex_unlock(&(coder->args->shared->mutex_wait));
}

void	lock_unlock_dongle(t_coder *coder, int dongle1, int dongle2, int lock)
{
	if (lock)
	{
		pthread_mutex_lock(&(coder->dongles[dongle1].mutex));
		pthread_mutex_lock(&(coder->dongles[dongle2].mutex));
		return ;
	}
	pthread_mutex_unlock(&(coder->dongles[dongle1].mutex));
	pthread_mutex_unlock(&(coder->dongles[dongle2].mutex));
}
