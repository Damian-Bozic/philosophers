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


typedef struct s_fork
{
	pthread_mutex_t	*mutex;
	struct s_fork	*next;
}	t_fork;

typedef struct s_philo
{
	pthread_t		thread;
	int				index;
	struct timeval	*last_meal;
	struct timeval	*sim_start;
	t_fork			*left_hand;
	t_fork			*right_hand;
	int				meals_eaten;
	struct s_philo	*next;
}	t_philo;


// if n_of_philos = 0 then sim stop
typedef struct s_sim
{
	int				n_of_philos;
	struct timeval	*sim_start;
	pthread_mutex_t	*death_mutex;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				min_eat_amount;
	t_philo			*philos;
	t_fork			*forks;
}	t_sim;

#endif