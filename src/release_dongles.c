/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   release_dongles.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/04 17:15:41 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/08 15:04:04 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_burnout(t_args *args)
{
	int	burnout;

	pthread_mutex_lock(&args->burnout_flag_mutex);
	burnout = args->burnout_flag;
	pthread_mutex_unlock(&args->burnout_flag_mutex);
	return (burnout);
}

void	heap_swap(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	put_dongle(t_dongle *dongle, int cooldown)
{
	long	now;

	now = get_time_on_ms(NULL);
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	dongle->available_at = now + cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

void	release_dongles(t_coder *coder)
{
	int	cooldown;

	cooldown = coder->args->dongle_cooldown;
	put_dongle(&coder->dongles[coder->first_dg], cooldown);
	put_dongle(&coder->dongles[coder->second_dg], cooldown);
}

void	lock_unlock_dongles(t_dongle *dg1, t_dongle *dg2, int lock)
{
	if (lock)
	{
		pthread_mutex_lock(&dg1->lock);
		pthread_mutex_lock(&dg2->lock);
		return ;
	}
	pthread_mutex_unlock(&dg1->lock);
	pthread_mutex_unlock(&dg2->lock);
}
