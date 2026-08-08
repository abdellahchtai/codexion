/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 17:25:35 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/08 14:39:10 by abchtaib         ###   ########.fr       */
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

typedef struct coder	t_coder;

typedef struct args
{
	int					nb_of_coders;
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					nb_of_compiles_required;
	int					dongle_cooldown;
	int					finished_coders;
	int					burnout_flag;
	char				*scheduler;
	long				start_simu;
	long				fifo_order;
	pthread_t			burnout_thread;
	pthread_mutex_t		mtx_fifo;
	pthread_mutex_t		finished_mutex;
	pthread_mutex_t		print_mutex;
	pthread_mutex_t		burnout_flag_mutex;
}						t_args;

typedef struct request
{
	int					coder_id;
	long				key;
	long				tie_breaker;
	int					id;
}						t_request;

typedef struct dongle
{
	int					available;
	int					heap_size;
	int					heap_capacity;
	long				available_at;
	t_request			*heap;
	pthread_cond_t		cond;
	pthread_mutex_t		lock;
}						t_dongle;

typedef struct coder
{
	int					first_dg;
	int					second_dg;
	int					coder_id;
	int					finished;
	long				last_compile;
	t_args				*args;
	t_dongle			*dongles;
	pthread_t			thread_id;
	pthread_mutex_t		mutex_last_compile;
}						t_coder;

int						init_args(char **av, t_args *args);
int						init_all(t_coder **coders, t_dongle **dongles,
							t_args *args);
int						get_both_dongles(t_coder *coder);
int						is_burnout(t_args *args);
int						heap_push(t_dongle *dongle, t_request request);
long					get_time_on_ms(t_coder *coder);
void					*coders_routine(void *args);
void					release_dongles(t_coder *coder);
void					*burnout_checker(void *args);
void					ft_clean_up(t_coder *coders);
void					ft_printf_mutex(t_coder *coder, char *str, int burnout);
void					ft_sleep(long time_to_sleep, t_args *args);
void					clean_dongle(t_dongle *dongles, int nb_of_dongles);
void					heap_swap(t_request *a, t_request *b);
void					lock_unlock_dongles(t_dongle *dg1, t_dongle *dg2,
							int lock);
void					destroy_shared_rsc(t_args *args);
void					wait_dongles(t_dongle *first, t_dongle *second);
t_request				heap_pop(t_dongle *dongle);

#endif
