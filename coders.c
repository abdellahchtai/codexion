/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:35:57 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:10:40 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	refactor(t_coder *coder)
{
	if (is_burnout(coder->args))
		return ;
	ft_printf_mutex(coder, "is refactoring", coder->coder_id);
	ft_sleep(coder->args->time_to_refactor, coder->args);
}

void	debug(t_coder *coder)
{
	if (is_burnout(coder->args))
		return ;
	ft_printf_mutex(coder, "is debugging", coder->coder_id);
	ft_sleep(coder->args->time_to_debug, coder->args);
}

void	start_compile(t_coder *coder, int first, int last)
{
	long	time_now;
	int		i;

	if (is_burnout(coder->args))
		return ;
	i = 0;
	while (i++ < 2)
		ft_printf_mutex(coder, "has taken a dongle", coder->coder_id);
	pthread_mutex_lock(&(coder->mutex_last_compile));
	coder->last_compile = get_time_on_ms(NULL, 0);
	pthread_mutex_unlock(&(coder->mutex_last_compile));
	ft_printf_mutex(coder, "is compiling", coder->coder_id);
	ft_sleep(coder->args->time_to_compile, coder->args);
	time_now = get_time_on_ms(NULL, 0);
	coder->dongles[first].available_at = time_now
		+ coder->args->dongle_cooldown;
	coder->dongles[last].available_at = time_now + coder->args->dongle_cooldown;
}

void	compile(t_coder *coder)
{
	int	dongle1;
	int	dongle2;

	get_dongle_order(*coder, &dongle1, &dongle2);
	while (!is_burnout(coder->args))
	{
		lock_unlock_dongle(coder, dongle1, dongle2, 1);
		if (is_burnout(coder->args))
			return (lock_unlock_dongle(coder, dongle1, dongle2, 0));
		if (dongle_ready(coder->dongles, dongle1, dongle2))
		{
			start_compile(coder, dongle1, dongle2);
			lock_unlock_dongle(coder, dongle1, dongle2, 0);
			pthread_mutex_lock(&coder->args->shared->mutex_wait);
			pthread_cond_broadcast(&coder->args->shared->cond_wait);
			pthread_mutex_unlock(&coder->args->shared->mutex_wait);
			return ;
		}
		else
		{
			lock_unlock_dongle(coder, dongle1, dongle2, 0);
			wait_for_dongles(coder, dongle1, dongle2);
		}
	}
}

void	*coders_routine(void *args)
{
	t_coder	*coder;
	int		repeat;

	coder = (t_coder *)args;
	repeat = coder->args->nb_of_compiles_required;
	while (repeat && !is_burnout(coder->args))
	{
		if (is_burnout(coder->args))
			return (NULL);
		compile(coder);
		if (is_burnout(coder->args))
			return (NULL);
		debug(coder);
		if (is_burnout(coder->args))
			return (NULL);
		refactor(coder);
		if (is_burnout(coder->args))
			return (NULL);
		repeat--;
	}
	pthread_mutex_lock(&(coder->args->shared->finished_mutex));
	coder->args->shared->finished_coders++;
	pthread_mutex_unlock(&(coder->args->shared->finished_mutex));
	return (NULL);
}
