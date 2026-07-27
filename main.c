/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:23:31 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:20:27 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_shared	shared;
	t_args		args;
	t_coder		*coders;
	t_dongle	*dongles;
	t_cleaner	cleanup;

	if (ac - 1 != 8)
		return (fprintf(stderr, "Error: Expecting 8 arguments.Given %d.\n", ac
				- 1));
	if (!init_args(av, &args, &shared))
		return (1);
	if (!init_all(&coders, &dongles, &args, &cleanup))
		return (ft_clean_up(cleanup), 1);
	ft_clean_up(cleanup);
	return (0);
}
