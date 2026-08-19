#include "codexion.h"

void	cleanup_sim(t_sim *sim)
{
	pthread_mutex_destroy(&sim->lock);
	pthread_mutex_destroy(&sim->print_lock);
	pthread_cond_destroy(&sim->wake);
	heap_destroy(&sim->queue);
	free(sim->coders);
	free(sim->dongles);
}

int	main(int ac, char **av)
{
	t_args	args;
	t_sim	sim;

	if (parse_args(ac, av, &args))
		return (1);
	if (validate_args(&args))
		return (1);
	if (init_sim(&sim, args))
	{
		fprintf(stderr, "Error: could not initialize simulation\n");
		return (1);
	}
	if (create_coder_threads(&sim) || create_monitor_thread(&sim))
	{
		fprintf(stderr, "Error: could not create threads\n");
		cleanup_sim(&sim);
		return (1);
	}
	join_coder_threads(&sim);
	join_monitor_thread(&sim);
	cleanup_sim(&sim);
	return (0);
}
