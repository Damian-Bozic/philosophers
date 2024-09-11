/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 15:43:31 by dbozic            #+#    #+#             */
/*   Updated: 2024/07/10 15:43:46 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H
# include <unistd.h>
# include "stdio.h"
# include "stdlib.h"
# include <sys/time.h>
# include <pthread.h>
# include <string.h>

typedef struct s_philo
{
	pthread_t		thread;
	pthread_mutex_t	*left_hand;
	pthread_mutex_t	*right_hand;
	int				index;
	int				meals_eaten;
	struct s_sim	*sim;
	struct timeval	*last_meal;
	struct timeval	*current_time;
	struct s_philo	*next;
}	t_philo;

typedef struct s_sim
{
	pthread_t		death_tracker;
	pthread_mutex_t	*death_mutex;
	t_philo			*philos;
	int				n_of_philos;
	int				hungriest_philo;
	int				has_a_philo_died;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				min_eat_amount;
	struct timeval	*earliest_meal;
	struct timeval	*sim_start;
}	t_sim;

// core functions:
t_sim	*init_struct(char **argv);
void	*start_thread(void *sent);
void	*death_timer_thread_start(void *sent);

// helper functions:
int		custom_atoi(char *str);
long	get_time_diff_ms(struct timeval *start, struct timeval *current);
int		check_if_everyone_has_finished_eating(t_sim *sim);
void	find_hungriest(t_sim *sim);
void	*free_all(t_sim *sim);
void	*free_philos(t_philo *philo_one);

#endif