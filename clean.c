/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 11:59:29 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:27:28 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_shared_rsc(t_shared *shared)
{
	pthread_mutex_destroy(&shared->mutex_wait);
	pthread_mutex_destroy(&shared->finished_mutex);
	pthread_mutex_destroy(&shared->print_mutex);
	pthread_mutex_destroy(&shared->burnout_flag_mutex);
	pthread_cond_destroy(&shared->cond_wait);
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

void	ft_clean_up(t_cleaner cleanup)
{
	t_coder	*coders;
	int		i;

	coders = cleanup.coders;
	i = 0;
	if (!coders)
		return ;
	pthread_join(coders->args->shared->burnout_thread, NULL);
	joining_threads(coders, coders->args->nb_of_coders);
	while (i < coders->args->nb_of_coders)
	{
		pthread_mutex_destroy(&(coders->dongles[i].mutex));
		pthread_mutex_destroy(&coders[i].mutex_last_compile);
		i++;
	}
	destroy_shared_rsc(coders->args->shared);
	free(cleanup.coders);
	free(cleanup.dongels);
}
