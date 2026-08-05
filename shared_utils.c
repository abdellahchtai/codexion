/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/05 17:22:03 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	ft_sleep(long time_to_sleep, t_args *args)
{
	long	start;

	start = get_time_on_ms(NULL);
	while (get_time_on_ms(NULL) - start < time_to_sleep)
	{
		if (is_burnout(args))
			return ;
		usleep(500);
	}
}

void	ft_printf_mutex(t_coder *coder, char *str, int burnout)
{
	pthread_mutex_lock(&coder->args->print_mutex);
	if (!is_burnout(coder->args) || burnout)
		printf("%ld %d %s\n", get_time_on_ms(coder), coder->coder_id, str);
	pthread_mutex_unlock(&coder->args->print_mutex);
}

long	get_time_on_ms(t_coder *coder)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	if (!coder)
		return (time.tv_sec * 1000 + time.tv_usec / 1000);
	return ((time.tv_sec * 1000 + time.tv_usec / 1000)
		- coder->args->start_simu);
}

void	wait_dongles(t_dongle *first, t_dongle *second)
{
	struct timeval	tv;
	struct timespec	ts;

	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec;
	ts.tv_nsec = tv.tv_usec * 1000 + 2000000;
	pthread_mutex_lock(&first->lock);
	pthread_cond_timedwait(&first->cond, &first->lock, &ts);
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_lock(&second->lock);
	pthread_cond_timedwait(&second->cond, &second->lock, &ts);
	pthread_mutex_unlock(&second->lock);
}
