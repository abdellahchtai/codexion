/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shared_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:29:43 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/03 15:01:44 by abchtaib         ###   ########.fr       */
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

void	ft_printf_mutex(t_coder *coder, char *str)
{
	pthread_mutex_lock(&coder->args->print_mutex);
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

long	get_next_ticket(t_args *args)
{
	long	ticket;

	pthread_mutex_lock(&args->mtx_fifo);
	ticket = args->fifo_order++;
	pthread_mutex_unlock(&args->mtx_fifo);
	return (ticket);
}