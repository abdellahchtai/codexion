/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders_routine_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 16:33:54 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/21 15:10:08 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	dongle_ready(t_dongle *dongle, int first, int second)
{
	long	time_now;

	time_now = get_time_on_ms(NULL, 0);
	return (dongle[first].available_at <= time_now
		&& dongle[second].available_at <= time_now);
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

void	get_dongle_order(t_coder coder, int *first, int *last)
{
	int	right_dg;
	int	left_dg;

	left_dg = coder.coder_id - 1;
	if (coder.coder_id == 1)
		right_dg = coder.args->nb_of_coders - 1;
	else
		right_dg = coder.coder_id - 2;
	if (right_dg < left_dg)
	{
		*first = right_dg;
		*last = left_dg;
	}
	else
	{
		*first = left_dg;
		*last = right_dg;
	}
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
