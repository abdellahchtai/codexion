/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:23:31 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/26 19:40:41 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
