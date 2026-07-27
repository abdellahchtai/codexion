/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:25:35 by abchtaib          #+#    #+#             */
/*   Updated: 2026/07/27 12:26:48 by abchtaib         ###   ########.fr       */
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

typedef struct shared
{
	int				finished_coders;
	int				burnout_flag;
	pthread_t		burnout_thread;
	pthread_cond_t	cond_wait;
	pthread_mutex_t	mutex_wait;
	pthread_mutex_t	finished_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	burnout_flag_mutex;
}					t_shared;

typedef struct args
{
	int				nb_of_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				nb_of_compiles_required;
	int				dongle_cooldown;
	char			*scheduler;
	long			start_simu;
	t_shared		*shared;
}					t_args;

typedef struct dongle
{
	long			available_at;
	pthread_mutex_t	mutex;
}					t_dongle;

typedef struct coder
{
	int				coder_id;
	long			last_compile;
	t_args			*args;
	t_dongle		*dongles;
	pthread_t		thread_id;
	pthread_mutex_t	mutex_last_compile;
}					t_coder;

typedef struct cleaner
{
	t_coder			*coders;
	t_dongle		*dongels;
}					t_cleaner;

int					init_args(char **av, t_args *args, t_shared *shared);
int					init_all(t_coder **coders, t_dongle **dongles, t_args *args,
						t_cleaner *cleanup);
void				*coders_routine(void *args);
void				get_dongle_order(t_coder coder, int *first, int *last);
long				get_time_on_ms(t_coder *coder, int time_stamp_flag);
int					dongle_ready(t_dongle *dongle, int first, int second);
void				wait_for_dongles(t_coder *coder, int dg1, int dg2);
void				*burnout_checker(void *args);
void				ft_clean_up(t_cleaner cleanup);
void				ft_printf_mutex(t_coder *coders, char *str,
						int coders_index);
int					is_burnout(t_args *args);
void				ft_sleep(long time_to_sleep, t_args *args);
void				lock_unlock_dongle(t_coder *coder, int dongle1,
						int dongle2, int lock);

#endif
