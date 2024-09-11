/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbozic <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 18:20:16 by dbozic            #+#    #+#             */
/*   Updated: 2024/09/11 18:20:17 by dbozic           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	custom_atoi(char *str)
{
	int		i;
	long	rtn;

	if (!str)
		return (-1);
	i = 0;
	rtn = 0;
	if (str[0] == '0' && str[1] == 0)
		return (0);
	if (str[0] == '0' && str[1] != 0)
		return (-1);
	while (str[i])
	{
		if (str[i] < 48 || str[i] > 57)
			return (-2);
		rtn = (rtn + (str[i] - '0'));
		i++;
		if (str[i])
			rtn = rtn * 10;
		if (rtn > 2147483647)
			return (-1);
	}
	return ((int)rtn);
}

long	get_time_diff_ms(struct timeval *start, struct timeval *current)
{
	return (((current->tv_sec - start->tv_sec) * 1000)
		+ ((current->tv_usec - start->tv_usec) / 1000));
}

int	check_if_everyone_has_finished_eating(t_sim *sim)
{
	int		i;
	int		flag;
	t_philo	*current;

	i = 0;
	flag = 1;
	current = sim->philos;
	if (sim->min_eat_amount == -1)
		return (0);
	while (i == 0 || current != sim->philos)
	{
		if (current->meals_eaten < sim->min_eat_amount)
			flag = 0;
		current = current->next;
		i++;
	}
	return (flag);
}

void	find_hungriest(t_sim *sim)
{
	int		i;
	int		time;
	t_philo	*current;

	i = 0;
	time = 0;
	current = sim->philos;
	while (i == 0 || current != sim->philos)
	{
		gettimeofday(current->current_time, NULL);
		if (get_time_diff_ms(current->last_meal, current->current_time) > time)
		{
			time = get_time_diff_ms(current->last_meal, current->current_time);
			sim->earliest_meal = current->last_meal;
			sim->hungriest_philo = current->index;
		}
		if (current->meals_eaten < sim->min_eat_amount)
		{
			sim->has_a_philo_died = 2;
			break ;
		}
		current = current->next;
		i++;
	}
	return ;
}
