/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 16:39:26 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:07:19 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_digit_or_sapce(char c, int flag)
{
	if (flag)
		return (c >= '0' && c <= '9');
	return ((c >= '\t' && c <= '\r') || c == ' ');
}

int	ft_atoi(char *str)
{
	long	nb;
	int		i;

	nb = 0;
	i = 0;
	while (is_digit_or_sapce(str[i], 0))
		i++;
	while (is_digit_or_sapce(str[i], 1))
	{
		nb = nb * 10 + str[i] - '0';
		if (nb > 2147483647)
			return (fprintf(stderr,
					"Error: This argument(%s) is to large for a int.\n", str),
				-1);
		i++;
	}
	return (nb);
}

int	check_args(char **av)
{
	int i, (j);
	j = 1;
	while (j < 8)
	{
		i = 0;
		while (av[j][i] && is_digit_or_sapce(av[j][i], 0))
			i++;
		if (!av[j][i])
			return (
				fprintf(stderr, "Error: Invalid argument at index %d: '%s'\n",
					j, av[j]), 0);
		while (av[j][i] && is_digit_or_sapce(av[j][i], 1))
			i++;
		while (av[j][i])
		{
			if (!(is_digit_or_sapce(av[j][i], 0)))
				return (
					fprintf(stderr, "Error: Invalid argument. index %d: '%s'\n"
						, j, av[j]), 0);
			i++;
		}
		j++;
	}
	return (1);
}

int	set_args_value(char **av, int *numbers)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		numbers[i] = ft_atoi(av[i + 1]);
		if (numbers[i] < 0)
			return (0);
		i++;
	}
	return (1);
}

int	init_args(char **av, t_args *args, t_shared *shared)
{
	int	numbers[7];

	if (!check_args(av) || !set_args_value(av, numbers))
		return (0);
	args->nb_of_coders = numbers[0];
	if (args->nb_of_coders < 2)
		return (printf("Error: Coders must be more than 1.\n"), 0);
	args->time_to_burnout = numbers[1];
	args->time_to_compile = numbers[2];
	args->time_to_debug = numbers[3];
	args->time_to_refactor = numbers[4];
	args->nb_of_compiles_required = numbers[5];
	args->dongle_cooldown = numbers[6];
	args->shared = shared;
	shared->finished_coders = 0;
	shared->burnout_flag = 0;
	pthread_mutex_init(&shared->mutex_wait, NULL);
	pthread_mutex_init(&shared->finished_mutex, NULL);
	pthread_mutex_init(&shared->print_mutex, NULL);
	pthread_mutex_init(&shared->burnout_flag_mutex, NULL);
	pthread_cond_init(&shared->cond_wait, NULL);
	if (!strcmp(av[8], "fifo") || !strcmp(av[8], "edf"))
		return (args->scheduler = av[8], 1);
	return (fprintf(stderr,
			"Error: Invalid scheduler '%s'.Use 'fifo' or 'edf'.\n", av[8]), 0);
}
