/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 11:59:29 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/03 12:56:39 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_shared_rsc(t_args *args)
{
	pthread_mutex_destroy(&args->finished_mutex);
	pthread_mutex_destroy(&args->print_mutex);
	pthread_mutex_destroy(&args->burnout_flag_mutex);
	pthread_mutex_destroy(&args->mtx_fifo);
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

void	clean_dongle(t_dongle *dongles, int nb_of_dongles)
{
	int	i;

	if (!dongles)
		return ;
	i = 0;
	while (i < nb_of_dongles)
	{
		pthread_cond_destroy(&dongles[i].cond);
		pthread_mutex_destroy(&dongles[i].lock);
		free(dongles[i].heap);
		i++;	
	}
	free(dongles);
}

void	clean_coders(t_coder *coders, int nb_of_coders)
{
	int	i;

	if (!coders)
		return ;
	i = 0;
	while (i < nb_of_coders)
	{
		pthread_mutex_destroy(&coders[i].mutex_last_compile);
		i++;
	}
}

void	ft_clean_up(t_coder *coders)
{
	if (coders)
	{
		joining_threads(coders, coders->args->nb_of_coders);
		pthread_join(coders->args->burnout_thread, NULL);
		clean_dongle(coders->dongles, coders->args->nb_of_coders);
		clean_coders(coders, coders->args->nb_of_coders);
		destroy_shared_rsc(coders->args);
		free(coders);
	}
}
