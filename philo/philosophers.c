/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/10 15:42:36 by dbozic            #+#    #+#             */
/*   Updated: 2024/07/10 15:43:11 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	*free_philos(t_philo *philo_one)
{
	t_philo	*current;
	t_philo	*next;
	int		i;

	i = 0;
	if (!philo_one)
		return (NULL);
	current = philo_one;
	while (current != philo_one || i == 0)
	{
		next = current->next;
		pthread_mutex_destroy(current->left_hand);
		free(current->current_time);
		free(current->left_hand);
		free(current->last_meal);
		free(current);
		current = next;
		i++;
	}
	return (NULL);
}

void	*free_all(t_sim *sim)
{
	if (sim->death_mutex)
	{
		pthread_mutex_destroy(sim->death_mutex);
		free(sim->death_mutex);
	}
	if (sim->sim_start)
		free(sim->sim_start);
	free_philos(sim->philos);
	free(sim);
	return (NULL);
}

void	close_sim(t_sim *sim)
{
	t_philo			*current;
	int				i;

	i = 0;
	current = sim->philos;
	pthread_join(sim->death_tracker, NULL);
	while (i < sim->n_of_philos)
	{
		pthread_join(current->thread, NULL);
		current = current->next;
		i++;
	}
	free_all(sim);
}

void	start_sim(t_sim *sim)
{
	t_philo			*current;
	int				i;

	i = 0;
	gettimeofday(sim->sim_start, NULL);
	sim->sim_start->tv_usec += 200000;
	current = sim->philos;
	while (i < sim->n_of_philos)
	{
		current->last_meal->tv_sec = sim->sim_start->tv_sec;
		current->last_meal->tv_usec = sim->sim_start->tv_usec;
		current = current->next;
		i++;
	}
	sim->earliest_meal = sim->sim_start;
	pthread_create(&sim->death_tracker, NULL, death_timer_thread_start, sim);
	i = 0;
	current = sim->philos;
	while (i < sim->n_of_philos)
	{
		pthread_create(&current->thread, NULL, start_thread, current);
		current = current->next;
		i++;
	}
	close_sim(sim);
}

int	main(int argc, char **argv)
{
	t_sim			*sim;

	sim = NULL;
	if (argc < 5 || argc > 6)
	{
		printf("ERROR\nIncorrect number of parameters\n");
		return (0);
	}
	sim = init_struct(argv);
	if (!sim)
		return (0);
	if (custom_atoi(argv[1]) <= 0 || sim->time_to_die < 0
		|| sim->time_to_eat < 0 || sim->time_to_sleep < 0
		|| sim->min_eat_amount < -1)
	{
		free_all(sim);
		printf("ERROR\nInvalid parameter(s)\n");
		return (0);
	}
	if (sim->min_eat_amount == 0)
	{
		free_all(sim);
		return (0);
	}
	start_sim(sim);
}
