/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/12 14:34:08 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/21 15:26:19 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	refactor(t_coder *coder)
{
	printf("%ld %d is refactoring\n", get_time_on_ms(coder, 1),
		coder->coder_id);
	usleep(coder->args->time_to_refactor * 1000);
}

void	debug(t_coder *coder)
{
	printf("%ld %d is debugging\n", get_time_on_ms(coder, 1), coder->coder_id);
	usleep(coder->args->time_to_debug * 1000);
}

void	start_compile(t_coder *coder, int first, int last)
{
	long	time_now;

	printf("%ld %d has taken a dongle\n", get_time_on_ms(coder, 1),
		coder->coder_id);
	printf("%ld %d has taken a dongle\n", get_time_on_ms(coder, 1),
		coder->coder_id);
	printf("%ld %d is compiling.\n", get_time_on_ms(coder, 1), coder->coder_id);
	usleep(coder->args->time_to_compile * 1000);
	time_now = get_time_on_ms(NULL, 0);
	coder->dongles[first].available_at = time_now
		+ coder->args->dongle_cooldown;
	coder->dongles[last].available_at = time_now + coder->args->dongle_cooldown;
	pthread_mutex_unlock(&(coder->dongles[first].mutex));
	pthread_mutex_unlock(&(coder->dongles[last].mutex));
}

void	compile(t_coder *coder)
{
	int				dongle1;
	int				dongle2;

	get_dongle_order(*coder, &dongle1, &dongle2);
	while (1)
	{
		pthread_mutex_lock(&(coder->dongles[dongle1].mutex));
		pthread_mutex_lock(&(coder->dongles[dongle2].mutex));
		if (dongle_ready(coder->dongles, dongle1, dongle2))
		{
			start_compile(coder, dongle1, dongle2);
			pthread_cond_broadcast(&coder->args->cond_wait);
			return ;
		}
		else
			wait_until(coder, dongle1, dongle2);
	}
}

void	*coders_routine(void *args)
{
	t_coder	*coder;
	int		repeat;

	coder = (t_coder *)args;
	repeat = coder->args->nb_of_compiles_required;
	while (repeat)
	{
		compile(coder);
		debug(coder);
		refactor(coder);
		repeat--;
	}
	return (NULL);
}
