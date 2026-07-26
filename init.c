/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 09:47:29 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/26 20:04:37 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_coder	*create_coders_dongles(int nb_of_creation, t_cleaner *cleanup,
		t_dongle **dongles)
{
	t_coder	*coders;

	cleanup->dongels = NULL;
	cleanup->coders = NULL;
	coders = malloc(sizeof(t_coder) * nb_of_creation);
	if (!coders)
		return (printf("Error: To many coders to create.\n"), NULL);
	cleanup->coders = coders;
	*dongles = malloc(sizeof(t_dongle) * nb_of_creation);
	if (!(*dongles))
		return (printf("Error: To man dongles to create.\n"), NULL);
	cleanup->dongels = *dongles;
	return (coders);
}

void	init_dongle(t_dongle *dongles, int limit)
{
	int	i;

	i = 0;
	while (i < limit)
	{
		dongles[i].available_at = 0;
		pthread_mutex_init(&(dongles[i].mutex), NULL);
		i++;
	}
}

void	init_coders(t_coder *coders, t_args *args, t_dongle *dongles, int limit)
{
	int	i;

	i = 0;
	while (i < limit)
	{
		coders[i].coder_id = i + 1;
		coders[i].last_compile = get_time_on_ms(NULL, 0);
		coders[i].args = args;
		coders[i].dongles = dongles;
		pthread_mutex_init(&(coders[i].mutex_last_compile), NULL);
		i++;
	}
}

int	create_coders_threads(t_coder *coders)
{
	int	i;

	i = 0;
	while (i < coders->args->nb_of_coders)
	{
		if (pthread_create(&(coders[i].thread_id), NULL, coders_routine,
				&(coders[i])))
			return (fprintf(stderr,
					"Error: Main thread faild to creat thread number %d.\n", i
					+ 1), coders->args->nb_of_coders = i, 0);
		i++;
	}
	return (1);
}

int	init_all(t_coder **coders, t_dongle **dongles, t_args *args,
		t_cleaner *cleanup)
{
	*coders = create_coders_dongles(args->nb_of_coders, cleanup, dongles);
	if (!(*coders))
		return (0);
	init_dongle(*dongles, args->nb_of_coders);
	init_coders(*coders, args, *dongles, args->nb_of_coders);
	(*coders)->args->start_simu = get_time_on_ms(NULL, 0);
	pthread_create(&args->burnout_thread, NULL, burnout_checker, *coders);
	return (create_coders_threads(*coders));
}
