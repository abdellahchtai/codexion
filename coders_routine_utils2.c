/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine_utils2.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 19:12:27 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/21 15:26:06 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	ft_max(t_dongle dongle1, t_dongle dongle2)
{
	if (dongle1.available_at >= dongle2.available_at)
		return (dongle1.available_at);
	return (dongle2.available_at);
}

void	wait_until(t_coder *coder, int dg1, int dg2)
{
	long			max;
	struct timespec	timeout;

	max = ft_max(coder->dongles[dg1], coder->dongles[dg2]);
	timeout.tv_sec = max / 1000;
	timeout.tv_nsec = max % 1000 * 1000000;
	pthread_mutex_unlock(&(coder->dongles[dg1].mutex));
	pthread_mutex_unlock(&(coder->dongles[dg2].mutex));
	pthread_mutex_lock(&(coder->args->mutex_wait));
	pthread_cond_timedwait(&(coder->args->cond_wait),
		&(coder->args->mutex_wait), &timeout);
	pthread_mutex_unlock(&(coder->args->mutex_wait));
}
