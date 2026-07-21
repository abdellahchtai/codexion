/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:23:31 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/21 15:10:21 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_clean_up(t_coder *coders, t_cleaner cleanup)
{
	int	i;

	i = 0;
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

int	main(int ac, char **av)
{
	t_args		args;
	t_coder		*coders;
	t_dongle	*dongles;
	t_cleaner	cleanup;

	if (ac - 1 != 8)
		return (fprintf(stderr, "Error: Expecting 8 arguments.Given %d.\n", ac
				- 1));
	if (!init_args(av, &args))
		return (1);
	if (!init_all(&coders, &dongles, &args, &cleanup))
		return (ft_clean_up(coders, cleanup));
	ft_clean_up(coders, cleanup);
	return (0);
}
