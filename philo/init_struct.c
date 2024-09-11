/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_struct.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:15:53 by dbozic            #+#    #+#             */
/*   Updated: 2024/09/11 18:15:55 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static t_philo	*new_philo2(t_philo *new)
{
	struct timeval	*timeval1;
	struct timeval	*timeval2;

	timeval1 = malloc(sizeof(struct timeval));
	if (!timeval1)
	{
		pthread_mutex_destroy(new->left_hand);
		free(new->left_hand);
		free(new);
		return (NULL);
	}
	memset(timeval1, 0, sizeof(struct timeval));
	timeval2 = malloc(sizeof(struct timeval));
	if (!timeval2)
	{
		pthread_mutex_destroy(new->left_hand);
		free(new->left_hand);
		free(new);
		free(timeval1);
	}
	memset(timeval2, 0, sizeof(struct timeval));
	new->last_meal = timeval1;
	new->current_time = timeval2;
	return (new);
}

static t_philo	*new_philo(int i, t_sim *sim)
{
	t_philo			*new;
	pthread_mutex_t	*new_fork;

	new = malloc(sizeof(t_philo));
	if (!new)
		return (NULL);
	new_fork = malloc(sizeof(pthread_mutex_t));
	if (!new_fork)
	{
		free(new);
		return (NULL);
	}
	if (pthread_mutex_init(new_fork, NULL) != 0)
	{
		free(new);
		free(new_fork);
		return (NULL);
	}
	new->index = i;
	new->meals_eaten = 0;
	new->left_hand = new_fork;
	new->sim = sim;
	return (new_philo2(new));
}

static t_philo	*init_philos(int n, t_sim *sim)
{
	t_philo	*philo_one;
	t_philo	*philo_next;
	t_philo	*philo_current;
	int		i;

	i = 1;
	philo_one = new_philo(0, sim);
	if (!philo_one)
		return (NULL);
	philo_current = philo_one;
	while (i < n)
	{
		philo_next = new_philo(i, sim);
		if (!philo_next)
			return (free_philos(philo_one));
		philo_current->next = philo_next;
		philo_next->right_hand = philo_current->left_hand;
		philo_current = philo_next;
		i++;
	}
	philo_current->next = philo_one;
	philo_one->right_hand = philo_current->left_hand;
	return (philo_one);
}

static int	init_struct2(t_sim *sim, char **argv)
{
	struct timeval	*timeval1;

	timeval1 = malloc(sizeof(struct timeval));
	if (!timeval1)
	{
		return (0);
	}
	memset(timeval1, 0, sizeof(struct timeval));
	sim->sim_start = timeval1;
	sim->earliest_meal = NULL;
	sim->hungriest_philo = 0;
	sim->has_a_philo_died = 0;
	sim->n_of_philos = custom_atoi(argv[1]);
	sim->time_to_die = custom_atoi(argv[2]);
	sim->time_to_eat = custom_atoi(argv[3]);
	sim->time_to_sleep = custom_atoi(argv[4]);
	sim->min_eat_amount = custom_atoi(argv[5]);
	return (1);
}

t_sim	*init_struct(char **argv)
{
	t_sim			*sim;
	pthread_mutex_t	*mutex;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	sim->earliest_meal = NULL;
	sim->sim_start = NULL;
	mutex = malloc(sizeof(pthread_mutex_t));
	if (!mutex)
		return (free_all(sim));
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		free(mutex);
		return (free_all(sim));
	}
	sim->death_mutex = mutex;
	if (!init_struct2(sim, argv))
		return (free_all(sim));
	sim->philos = init_philos(custom_atoi(argv[1]), sim);
	if (!sim->philos)
		return (free_all(sim));
	return (sim);
}
