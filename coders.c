/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/26 19:35:57 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/03 20:25:18 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	refactor(t_coder *coder)
{
	if (is_burnout(coder->args))
		return ;
	ft_printf_mutex(coder, "is refactoring");
	ft_sleep(coder->args->time_to_refactor, coder->args);
}

void	debug(t_coder *coder)
{
	if (is_burnout(coder->args))
		return ;
	ft_printf_mutex(coder, "is debugging");
	ft_sleep(coder->args->time_to_debug, coder->args);
}

int	start_compile(t_coder *coder)
{
	if (is_burnout(coder->args))
		return (0);

	pthread_mutex_lock(&(coder->mutex_last_compile));
	coder->last_compile = get_time_on_ms(NULL);
	pthread_mutex_unlock(&(coder->mutex_last_compile));

	ft_printf_mutex(coder, "is compiling");
	ft_sleep(coder->args->time_to_compile, coder->args);
	return (1);
}

int	compile(t_coder *coder)
{
	if (!get_both_dongles(coder))
		return (0);
	if (!start_compile(coder))
		return (release_dongles(coder), 0);
	release_dongles(coder);
	return (1);
}

void	*coders_routine(void *args)
{
	t_coder	*coder;
	int		repeat;

	coder = (t_coder *)args;
	repeat = coder->args->nb_of_compiles_required;
	while (repeat && !is_burnout(coder->args))
	{
		if (!compile(coder))
			return (NULL);
		debug(coder);
		if (is_burnout(coder->args))
			return (NULL);
		refactor(coder);
		if (is_burnout(coder->args))
			return (NULL);
		repeat--;
	}
	coder->finished = 1;
	pthread_mutex_lock(&(coder->args->finished_mutex));
	coder->args->finished_coders++;
	pthread_mutex_unlock(&(coder->args->finished_mutex));
	return (NULL);
}
