#include "codexion.h"

static void	init_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		sim->dongles[i].holder = -1;
		sim->dongles[i].available_time = sim->start_time;
		i++;
	}
}

static void	init_coders(t_sim *sim)
{
	int	i;
	int	n;

	n = sim->args.number_of_coders;
	i = 0;
	while (i < n)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].left_dongle = i;
		sim->coders[i].right_dongle = (i + 1) % n;
		sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].compiles_done = 0;
		sim->coders[i].finished = 0;
		sim->coders[i].sim = sim;
		i++;
	}
}

static int	init_sync(t_sim *sim)
{
	if (pthread_mutex_init(&sim->lock, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->print_lock, NULL) != 0)
		return (1);
	if (pthread_cond_init(&sim->wake, NULL) != 0)
		return (1);
	return (0);
}

int	init_sim(t_sim *sim, t_args args)
{
	sim->args = args;
	sim->coders = malloc(sizeof(t_coder) * args.number_of_coders);
	sim->dongles = malloc(sizeof(t_dongle) * args.number_of_coders);
	if (!sim->coders || !sim->dongles)
	{
		free(sim->coders);
		free(sim->dongles);
		return (1);
	}
	sim->start_time = get_time_in_ms();
	sim->stop = 0;
	if (init_sync(sim))
		return (1);
	if (heap_init(&sim->queue, args.number_of_coders))
		return (1);
	init_dongles(sim);
	init_coders(sim);
	return (0);
}
