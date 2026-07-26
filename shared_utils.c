/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/26 20:04:56 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	ft_sleep(long time_to_sleep, t_args *args)
{
	long	start;

	start = get_time_on_ms(NULL, 0);
	while (get_time_on_ms(NULL, 0) - start < time_to_sleep)
	{
		if (is_burnout(args))
			return ;
		usleep(500);
	}
}

void	ft_printf_mutex(t_coder *coders, char *str, int coders_index)
{
	pthread_mutex_lock(&coders->args->print_mutex);
	printf("%ld %d %s\n", get_time_on_ms(coders, 1), coders_index, str);
	pthread_mutex_unlock(&coders->args->print_mutex);
}

void	joining_threads(t_coder *coders, int nb_of_coders)
{
	int	i;

	i = 0;
	while (i < nb_of_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
}

long	get_time_on_ms(t_coder *coder, int time_stamp_flag)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	if (!time_stamp_flag)
		return (time.tv_sec * 1000 + time.tv_usec / 1000);
	return ((time.tv_sec * 1000 + time.tv_usec / 1000)
		- coder->args->start_simu);
}

int	ft_clean_up(t_coder *coders, t_cleaner cleanup)
{
	int	i;

	i = 0;
	pthread_join(coders->args->burnout_thread, NULL);
	joining_threads(coders, coders->args->nb_of_coders);
	while (i < coders->args->nb_of_coders)
	{
		pthread_mutex_destroy(&(coders->dongles[i].mutex));
		i++;
	}
	pthread_mutex_destroy(&(coders->args->mutex_wait));
	pthread_cond_destroy(&(coders->args->cond_wait));
	free(cleanup.coders);
	free(cleanup.dongels);
	return (1);
}
