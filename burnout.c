/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   burnout.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:04:16 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/03 15:01:26 by abchtaib         ###   ########.fr       */
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
	return (finished != coders->args->nb_of_coders);
}

int	check_last_compilation(t_coder *coder)
{
	int		flag;
	long	now;

	if (coder->finished)
		return (0);

	flag = 0;
	pthread_mutex_lock(&(coder->mutex_last_compile));
	now = get_time_on_ms(NULL);
	if (now - coder->last_compile >= coder->args->time_to_burnout)
		flag = 1;
	pthread_mutex_unlock(&(coder->mutex_last_compile));
	return (flag);
}

void	broadcast_all_coders(t_coder *coders)
{
	int	i;

	i = 0;
	while (i < coders->args->nb_of_coders)
	{
		pthread_mutex_lock(&coders->dongles[i].lock);
		pthread_cond_broadcast(&coders->dongles[i].cond);
		pthread_mutex_unlock(&coders->dongles[i].lock);
		i++;
	}
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
				ft_printf_mutex(&coders[i], "burned out");
				broadcast_all_coders(coders);
				return (NULL);
			}
			i++;
		}
		usleep(500);
	}
}
