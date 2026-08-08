/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/08 18:59:16 by abchtaib         ###   ########.fr       */
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
	struct timespec	ts;
	long			target_ms;
	long			now_ms;

	now_ms = get_time_on_ms(NULL);
	pthread_mutex_lock(&first->lock);
	target_ms = first->available_at;
	pthread_mutex_unlock(&first->lock);
	pthread_mutex_lock(&second->lock);
	if (second->available_at > target_ms)
		target_ms = second->available_at;
	pthread_mutex_unlock(&second->lock);
	if (target_ms <= now_ms)
		target_ms = now_ms + 1;
	ts.tv_sec = target_ms / 1000;
	ts.tv_nsec = (target_ms % 1000) * 1000000;
	pthread_mutex_lock(&first->lock);
	pthread_cond_timedwait(&first->cond, &first->lock, &ts);
	pthread_mutex_unlock(&first->lock);
}
