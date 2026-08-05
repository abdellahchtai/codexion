/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 18:23:31 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/05 17:25:06 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int ac, char **av)
{
	t_args		args;
	t_coder		*coders;
	t_dongle	*dongles;

	if (ac - 1 != 8)
		return (fprintf(stderr, "Error: Expecting 8 arguments.Given %d.\n", ac
				- 1));
	if (!init_args(av, &args))
		return (1);
	if (!init_all(&coders, &dongles, &args))
		return (1);
	ft_clean_up(coders);
	return (0);
}
