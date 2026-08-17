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
