/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:04:16 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/26 20:03:38 by abchtaib         ###   ########.fr       */
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

void	set_burnout_flag(t_args *args)
{
	pthread_mutex_lock(&args->burnout_flag_mutex);
	args->burnout_flag = 1;
	pthread_mutex_unlock(&args->burnout_flag_mutex);
}

int	is_coders_finish(t_coder *coders)
{
	int	finished;

	pthread_mutex_lock(&(coders->args->finished_mutex));
	finished = coders->args->finished_coders;
	pthread_mutex_unlock(&(coders->args->finished_mutex));
	if (finished == coders->args->nb_of_coders)
		return (0);
	return (1);
}

int	check_last_compilation(t_coder coder)
{
	long	now;

	now = get_time_on_ms(NULL, 0);
	return (now - coder.last_compile >= coder.args->time_to_burnout);
}

void	*burnout_checker(void *args)
{
	t_coder	*coders;
	int		i;

	coders = args;
	while (1)
	{
		i = 0;
		while (i < coders->args->nb_of_coders)
		{
			pthread_mutex_lock(&(coders[i].mutex_last_compile));
			if (check_last_compilation(coders[i]))
			{
				pthread_mutex_unlock(&(coders[i].mutex_last_compile));
				set_burnout_flag(coders->args);
				ft_printf_mutex(coders, "burned out", coders[i].coder_id);
				pthread_cond_broadcast(&(coders->args->cond_wait));
				return (NULL);
			}
			pthread_mutex_unlock(&(coders[i].mutex_last_compile));
			if (!is_coders_finish(coders))
				return (NULL);
			i++;
		}
		usleep(500);
	}
}
