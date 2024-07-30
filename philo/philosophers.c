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

int	custom_atoi(char *str)
{
	int		i;
	long	rtn;

	if (!str)
		return (-1);
	i = 0;
	rtn = 0;
	while (str[i])
	{
		if (str[i] < 48 || str[i] > 57)
			return (-1);
		rtn = (rtn + (str[i] - '0'));
		i++;
		if (str[i])
			rtn = rtn * 10;
		if (rtn > 2147483647)
			return (-1);
	}
	return ((int)rtn);
}

// have this return NULL
void	*free_all(t_sim *sim)
{
	t_philo	*current;
	t_fork	*f_current;
	t_philo	*next;
	t_fork	*f_next;

	if (!sim)
		return (NULL);
	if (sim->philos)
	{
		current = sim->philos;
		while (current)
		{
			next = current->next;
			if (current->left_hand)
			{
				printf("freeing left hand of %d", current->index);
				pthread_mutex_destroy(current->left_hand->mutex);
				free(current->left_hand->mutex);
				free(current->left_hand);
			}
			if (current->right_hand)
			{
				printf("freeing right hand of %d", current->index);
				pthread_mutex_destroy(current->right_hand->mutex);
				free(current->right_hand->mutex);
				free(current->right_hand);
			}
			free(current->last_meal);
			free(current->sim_start);
			free(current);
			current = next;
		}
	}
	if (sim->forks)
	{
		f_current = sim->forks;
		while (f_current)
		{
			f_next = f_current->next;
			printf("freeing table fork\n");
			pthread_mutex_destroy(f_current->mutex);
			free(f_current->mutex);
			free(f_current);
			f_current = f_next;
		}
	}
	if (sim->sim_start)
		free(sim->sim_start);
	if (sim->death_mutex)
	{
		pthread_mutex_destroy(sim->death_mutex);
		free(sim->death_mutex);
	}
	free(sim);
	return (NULL);
}

t_philo	*init_philo_variables(t_philo *new, int index)
{
	pthread_t		thread;
	struct timeval	*tv1;
	struct timeval	*tv2;

	thread = 0;
	tv1 = malloc(sizeof(struct timeval));
	if (!tv1)
		return (NULL);
	memset(tv1, 0, sizeof(struct timeval));
	tv2 = malloc(sizeof(struct timeval));
	if (!tv2)
	{
		free(tv1);
		return (NULL);
	}
	memset(tv2, 0, sizeof(struct timeval));
	new->last_meal = tv1;
	new->sim_start = tv2;
	new->thread = thread;
	new->index = index;
	new->meals_eaten = 0;
	new->left_hand = NULL;
	new->right_hand = NULL;
	new->next = NULL;
	return (new);
}

t_philo	*extent_philo_list(t_philo **lst, int index)
{
	t_philo			*new;
	static t_philo	*current = NULL;

	new = malloc(sizeof(t_philo));
	if (!new)
		return (NULL);
	if (!init_philo_variables(new, index))
	{
		free(new);
		return (NULL);
	}
	if (!*lst)
	{
		*lst = new;
		return (new);
	}
	if (!current)
		current = *lst;
	if (current->next)
		current = current->next;
	current->next = new;
	return (new);
}

t_philo	*init_philos(int n_of_philos)
{
	int		i;
	t_philo	*philos;
	t_philo	*next;

	i = 0;
	philos = NULL;
	while (i < n_of_philos)
	{
		if (!extent_philo_list(&philos, i))
		{
			while (philos)
			{
				next = philos->next;
				free(philos->last_meal);
				free(philos);
				philos = next;
			}
			return (NULL);
		}
		i++;
	}
	return (philos);
}

void	*init_variables(t_sim *sim, char **argv)
{
	struct timeval	*tv1;
	pthread_mutex_t *mutex;

	mutex = malloc(sizeof(pthread_mutex_t));
	if (!mutex)
		return (free_all(sim));
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		free(mutex);
		return (free_all(sim));
	}
	tv1 = malloc(sizeof(struct timeval));
	if (!tv1)
		return (free_all(sim));
	memset(tv1, 0, sizeof(struct timeval));
	sim->death_mutex = mutex;
	sim->sim_start = tv1;
	sim->n_of_philos = custom_atoi(argv[1]);
	sim->time_to_die = custom_atoi(argv[2]);
	sim->time_to_eat = custom_atoi(argv[3]);
	sim->time_to_sleep = custom_atoi(argv[4]);
	sim->min_eat_amount = custom_atoi(argv[5]);
	sim->philos = NULL;
	return (sim);
}

t_fork	*new_fork(void)
{
	t_fork			*new;
	pthread_mutex_t *mutex;

	new = malloc(sizeof(t_fork));
	if (!new)
		return (NULL);
	mutex = malloc(sizeof(pthread_mutex_t));
	if (!mutex)
	{
		free(new);
		return (NULL);
	}
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		free(new);
		free(mutex);
		return (NULL);
	}
	new->mutex = mutex;
	new->next = NULL;
	printf("success %p\n", new);
	return (new);
}

t_fork	*init_forks(int	n_of_philos)
{
	t_fork	*head;
	t_fork	*current;
	t_fork	*new;
	int		i;

	i = 0;
	head = NULL;
	while (i <= n_of_philos) // +1 fork for main mutex;
	{
		new = new_fork();
		if (!new)
		{
			while (head)
			{
				current = head->next;
				free(head);
				head = current;
			}
			return (NULL);
		}
		if (!head)
		{
			head = new;
			current = head;
		}
		else
		{
			current->next = new;
			current = current->next;
		}
		i++;
	}
	current = head;
	while (current)
	{
		usleep(10000);
		printf("found one %p with mutex %p\n", current, current->mutex);
		current = current->next;
		i++;
	}
	return (head);
}

t_sim	*init_struct(char **argv)
{
	t_sim	*sim;

	sim = malloc(sizeof(t_sim));
	if (!sim)
		return (NULL);
	if (!init_variables(sim, argv))
		return (free_all(sim));
	if (sim->n_of_philos <= 0 || sim->time_to_die == -1 || sim->time_to_eat ==
		-1 || sim->time_to_sleep == -1)
	{
		printf("Invalid parameter(s)\n");
		return (free_all(sim));
	}
	if (sim->n_of_philos > 10000)
	{
		printf("Too many philosophers\n");
		return (free_all(sim));
	}
	sim->philos = init_philos(sim->n_of_philos);
	if (!sim->philos)
		return (free_all(sim));
	sim->forks = init_forks(sim->n_of_philos);
	if (!sim->forks)
		return (free_all(sim));
	return (sim);
}

//
//	USEFUL vvv

// long	get_sim_time(t_sim *sim)
// {
// 	long	rtn;

// 	if (!sim->sim_start || !sim->sim_time)
// 		return (0);
// 	rtn = (sim->sim_time->tv_usec - sim->sim_start->tv_usec) / 1000;
// 	rtn += (sim->sim_time->tv_sec - sim->sim_start->tv_sec) * 1000;
// 	return (rtn);
// }

//	USEFUL ^^^
//

//
//	THREAD vvv
//

void	*philo_think(t_sim *sim, t_philo *philo)
{
	write(1, "Think\n", 9);

	return (NULL);
}

void	*start_thread(void *sent)
{
	// static t_sim	*sim = NULL;
	// t_philo			*philo;

	write(1, "START\n", 7);
	// if (!sim)
	// {
	// 	sim = *(t_sim **)sent;
	// 	return (NULL);
	// }
	// philo_think(sim, philo);
	return (NULL);
}

//
//	THREAD ^^^
//

int	main(int argc, char **argv)
{
	t_sim			*sim;
	t_philo			*current;
	pthread_t		first_thread;

	printf("MAIN\n");
	sim = NULL;
	if (argc < 5 || argc > 6)
	{
		printf("Incorrect amount of parameters\n");
		return (0);
	}

	sim = init_struct(argv);
	if (!sim)
		return (0);

	printf("philosophers = %d\n", sim->n_of_philos);
	printf("time_to_die = %dms\n", sim->time_to_die);
	printf("time_to_eat = %dms\n", sim->time_to_eat);
	printf("time_to_sleep = %dms\n", sim->time_to_sleep);
	printf("meals_required = %d\n", sim->min_eat_amount);

	if (!sim->philos)
		printf("no philosophers found\n");

	current = sim->philos;
	while (current)
	{
		printf("found philosopher %d", current->index + 1);
		printf(" using thread %p\n", &current->thread);
		current = current->next;
	}

	gettimeofday(sim->sim_start, NULL);
	sim->sim_start->tv_sec += 5;
	printf("Starting simulation in 5 seconds\n");
	printf("%lds, %ldus\n", sim->sim_start->tv_sec, sim->sim_start->tv_usec);

	pthread_create(&first_thread, NULL, start_thread, sim);
	pthread_join(first_thread, NULL);

	current = sim->philos;
	while (current)
	{
		current->last_meal->tv_sec = sim->sim_start->tv_sec;
		current->last_meal->tv_usec = sim->sim_start->tv_usec;
		// printf("told philo %d when to start (%ld)\n", current->index + 1,
			// current->last_meal->tv_sec);
		pthread_create(&current->thread, NULL, start_thread, current);
		current = current->next;
	}

	current = sim->philos;
	while (current)
	{
		pthread_join(current->thread, NULL);
		current = current->next;
	}
	
	printf("End of simulation\n");
	free_all(sim);
	return (1);
}
