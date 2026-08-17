#include "codexion.h"

int	create_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	join_coder_threads(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->args.number_of_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	return (0);
}
