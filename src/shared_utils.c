/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/09 18:56:09 by abchtaib         ###   ########.fr       */
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

void	wait_helper(t_dongle *dongle, int cooldown_wait)
{
	if (cooldown_wait)
	{
		usleep(cooldown_wait * 1000);
		return ;
	}
	pthread_mutex_lock(&dongle->lock);
	if (!dongle->available)
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	pthread_mutex_unlock(&dongle->lock);
}

void	wait_dongles(t_dongle *first, t_dongle *second)
{
	long	now_ms;
	long	target_ms;
	int		first_avail;
	int		second_avail;

	lock_unlock_dongles(first, second, 1);
	first_avail = first->available;
	second_avail = second->available;
	target_ms = first->available_at;
	if (second->available_at > target_ms)
		target_ms = second->available_at;
	lock_unlock_dongles(first, second, 0);
	now_ms = get_time_on_ms(NULL);
	if (!first_avail)
		wait_helper(first, 0);
	else if (!second_avail)
		wait_helper(second, 0);
	else if (target_ms > now_ms)
		wait_helper(NULL, target_ms - now_ms);
	else
	{
		pthread_mutex_lock(&first->lock);
		pthread_cond_wait(&first->cond, &first->lock);
		pthread_mutex_unlock(&first->lock);
	}
}
