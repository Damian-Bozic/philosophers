/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   death_tracker.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:24:04 by dbozic            #+#    #+#             */
/*   Updated: 2024/09/11 18:24:15 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

static void	death_timer_loop(t_sim *sim, struct timeval *current)
{
	while (1)
	{
		pthread_mutex_lock(sim->death_mutex);
		if (sim->has_a_philo_died == 1)
			break ;
		gettimeofday(current, NULL);
		if (get_time_diff_ms(sim->earliest_meal, current) >= sim->time_to_die)
		{
			sim->has_a_philo_died = 1;
			printf("%ld %d died\n", get_time_diff_ms(sim->sim_start, current),
				sim->hungriest_philo + 1);
			break ;
		}
		pthread_mutex_unlock(sim->death_mutex);
		usleep(1000);
	}
	free(current);
	pthread_mutex_unlock(sim->death_mutex);
}

void	*death_timer_thread_start(void *sent)
{
	t_sim			*sim;
	struct timeval	*current;

	sim = (t_sim *)sent;
	current = malloc(sizeof(struct timeval));
	if (!current)
	{
		pthread_mutex_lock(sim->death_mutex);
		sim->has_a_philo_died = 1;
		pthread_mutex_unlock(sim->death_mutex);
		return (NULL);
	}
	memset(current, 0, sizeof(struct timeval));
	death_timer_loop(sim, current);
	return (NULL);
}
