/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   life_cycle.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:21:55 by dbozic            #+#    #+#             */
/*   Updated: 2024/09/11 18:22:12 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static int	pickup_first_fork(t_philo *philo)
{
	if (philo->index % 2 == 0)
	{
		usleep(5000);
		pthread_mutex_lock(philo->left_hand);
	}
	else
		pthread_mutex_lock(philo->right_hand);
	pthread_mutex_lock(philo->sim->death_mutex);
	if (philo->sim->has_a_philo_died == 1)
	{
		if (philo->index % 2 == 0)
			pthread_mutex_unlock(philo->left_hand);
		else
			pthread_mutex_unlock(philo->right_hand);
		pthread_mutex_unlock(philo->sim->death_mutex);
		return (0);
	}
	gettimeofday(philo->current_time, NULL);
	printf("%ld %d has taken a fork\n", get_time_diff_ms(philo->sim->sim_start,
			philo->current_time), philo->index + 1);
	pthread_mutex_unlock(philo->sim->death_mutex);
	return (1);
}

static int	pickup_second_fork_and_eat(t_philo *philo)
{
	if (philo->index % 2 == 0)
		pthread_mutex_lock(philo->right_hand);
	else
		pthread_mutex_lock(philo->left_hand);
	pthread_mutex_lock(philo->sim->death_mutex);
	if (philo->sim->has_a_philo_died == 1)
	{
		pthread_mutex_unlock(philo->sim->death_mutex);
		pthread_mutex_unlock(philo->right_hand);
		pthread_mutex_unlock(philo->left_hand);
		return (0);
	}
	gettimeofday(philo->current_time, NULL);
	printf("%ld %d has taken a fork\n", get_time_diff_ms(philo->sim->sim_start,
			philo->current_time), philo->index + 1);
	printf("%ld %d is eating\n", get_time_diff_ms(philo->sim->sim_start,
			philo->current_time), philo->index + 1);
	gettimeofday(philo->last_meal, NULL);
	find_hungriest(philo->sim);
	pthread_mutex_unlock(philo->sim->death_mutex);
	usleep(philo->sim->time_to_eat * 1000);
	pthread_mutex_unlock(philo->right_hand);
	pthread_mutex_unlock(philo->left_hand);
	return (1);
}

static int	go_sleep(t_philo *philo)
{
	pthread_mutex_lock(philo->sim->death_mutex);
	if (philo->sim->has_a_philo_died == 1)
	{
		pthread_mutex_unlock(philo->sim->death_mutex);
		return (0);
	}
	gettimeofday(philo->current_time, NULL);
	printf("%ld %d is sleeping\n", get_time_diff_ms(philo->sim->sim_start,
			philo->current_time), philo->index + 1);
	philo->meals_eaten = philo->meals_eaten + 1;
	if (philo->sim->min_eat_amount != -1
		&& check_if_everyone_has_finished_eating(philo->sim) == 1)
		philo->sim->has_a_philo_died = 1;
	pthread_mutex_unlock(philo->sim->death_mutex);
	usleep(philo->sim->time_to_sleep * 1000);
	return (1);
}

static void	*life_cycle_loop(t_philo *philo)
{
	while (1)
	{
		pthread_mutex_lock(philo->sim->death_mutex);
		if (philo->sim->has_a_philo_died == 1)
		{
			pthread_mutex_unlock(philo->sim->death_mutex);
			break ;
		}
		gettimeofday(philo->current_time, NULL);
		printf("%ld %d is thinking\n", get_time_diff_ms(philo->sim->sim_start,
				philo->current_time), philo->index + 1);
		pthread_mutex_unlock(philo->sim->death_mutex);
		if (!pickup_first_fork(philo))
			break ;
		if (philo->left_hand == philo->right_hand)
		{
			pthread_mutex_unlock(philo->left_hand);
			break ;
		}
		if (!pickup_second_fork_and_eat(philo))
			break ;
		if (!go_sleep(philo))
			break ;
	}
	return (NULL);
}

void	*start_thread(void *sent)
{
	t_philo		*philo;
	static int	flag = 0;

	philo = (t_philo *)sent;
	pthread_mutex_lock(philo->sim->death_mutex);
	if (!philo->sim->earliest_meal)
		philo->sim->earliest_meal = philo->sim->sim_start;
	while (flag == 0)
	{
		gettimeofday(philo->current_time, NULL);
		if (get_time_diff_ms(philo->sim->sim_start, philo->current_time) >= 0)
		{
			flag = 1;
			break ;
		}
		usleep(1000);
	}
	find_hungriest(philo->sim);
	pthread_mutex_unlock(philo->sim->death_mutex);
	life_cycle_loop(philo);
	return (NULL);
}
