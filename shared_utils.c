/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:26:00 by abchtaib         ###   ########.fr       */
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
	pthread_mutex_lock(&coders->args->shared->print_mutex);
	printf("%ld %d %s\n", get_time_on_ms(coders, 1), coders_index, str);
	pthread_mutex_unlock(&coders->args->shared->print_mutex);
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
