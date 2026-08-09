/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/09 17:51:44 by abchtaib         ###   ########.fr       */
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

void    wait_dongles(t_dongle *first, t_dongle *second)
{
    long    now_ms;
    long    target_ms;
    int        first_avail;
    int        second_avail;

    lock_unlock_dongles(first, second, 1);
    first_avail = first->available;
    second_avail = second->available;
    target_ms = first->available_at;
    if (second->available_at > target_ms)
        target_ms = second->available_at;
    lock_unlock_dongles(first, second, 0);
    now_ms = get_time_on_ms(NULL);
    if (!first_avail)
    {
        pthread_mutex_lock(&first->lock);
        if (!first->available)
            pthread_cond_wait(&first->cond, &first->lock);
        pthread_mutex_unlock(&first->lock);
    }
    else if (!second_avail)
    {
        pthread_mutex_lock(&second->lock);
        if (!second->available)
            pthread_cond_wait(&second->cond, &second->lock);
        pthread_mutex_unlock(&second->lock);
    }
    else if (target_ms > now_ms)
    {
        long    diff;

        diff = target_ms - now_ms;
        if (diff > 0)
            usleep(diff * 1000);
    }
    else
    {
        pthread_mutex_lock(&first->lock);
        pthread_cond_wait(&first->cond, &first->lock);
        pthread_mutex_unlock(&first->lock);
    }
}