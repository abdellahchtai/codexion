/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:25:35 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/21 15:09:57 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

typedef struct args
{
	char			*scheduler;
	int				nb_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				nb_of_compiles_required;
	int				dongle_cooldown;
	long			start_simu;
	pthread_mutex_t	mutex_wait;
	pthread_cond_t	cond_wait;
}					t_args;

typedef struct dongle
{
	pthread_mutex_t	mutex;
	long			available_at;
}					t_dongle;

typedef struct coder
{
	pthread_t		thread_id;
	int				coder_id;
	t_args			*args;
	t_dongle		*dongles;
}					t_coder;

typedef struct cleaner
{
	t_coder			*coders;
	t_dongle		*dongels;
}					t_cleaner;

int					init_args(char **av, t_args *args);
int					init_all(t_coder **coders, t_dongle **dongles, t_args *args,
						t_cleaner *cleanup);
void				*coders_routine(void *args);
void				get_dongle_order(t_coder coder, int *first, int *last);
void				joining_threads(t_coder *coders, int nb_of_coders);
long				get_time_on_ms(t_coder *coder, int time_stamp_flag);
int					dongle_ready(t_dongle *dongle, int first, int second);
void				wait_until(t_coder *coder, int dg1, int dg2);

#endif
