/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:04:16 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:08:58 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_burnout(t_args *args)
{
	int	burnout;

	pthread_mutex_lock(&args->shared->burnout_flag_mutex);
	burnout = args->shared->burnout_flag;
	pthread_mutex_unlock(&args->shared->burnout_flag_mutex);
	return (burnout);
}

void	set_burnout_flag(t_args *args)
{
	pthread_mutex_lock(&args->shared->burnout_flag_mutex);
	args->shared->burnout_flag = 1;
	pthread_mutex_unlock(&args->shared->burnout_flag_mutex);
}

int	is_coders_finish(t_coder *coders)
{
	int	finished;

	pthread_mutex_lock(&(coders->args->shared->finished_mutex));
	finished = coders->args->shared->finished_coders;
	pthread_mutex_unlock(&(coders->args->shared->finished_mutex));
	return (finished != coders->args->nb_of_coders);
}

int	check_last_compilation(t_coder *coder)
{
	int		flag;
	long	now;

	flag = 0;
	pthread_mutex_lock(&(coder->mutex_last_compile));
	now = get_time_on_ms(NULL, 0);
	if (now - coder->last_compile >= coder->args->time_to_burnout)
		flag = 1;
	pthread_mutex_unlock(&(coder->mutex_last_compile));
	return (flag);
}

void	*burnout_checker(void *args)
{
	t_coder	*coders;
	int		i;

	coders = args;
	while (1)
	{
		i = 0;
		if (!is_coders_finish(coders))
			return (NULL);
		while (i < coders->args->nb_of_coders)
		{
			if (check_last_compilation(&coders[i]))
			{
				set_burnout_flag(coders->args);
				ft_printf_mutex(coders, "burned out", coders[i].coder_id);
				pthread_mutex_lock(&coders->args->shared->mutex_wait);
				pthread_cond_broadcast(&coders->args->shared->cond_wait);
				pthread_mutex_unlock(&coders->args->shared->mutex_wait);
				return (NULL);
			}
			i++;
		}
		usleep(500);
	}
}
