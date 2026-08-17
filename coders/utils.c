#include "codexion.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/* prints one log line; a dedicated mutex keeps lines from interleaving */
void	print_status(t_coder *coder, const char *msg)
{
	long	elapsed;

	pthread_mutex_lock(&coder->sim->print_lock);
	elapsed = get_time_in_ms() - coder->sim->start_time;
	printf("%ld %d %s\n", elapsed, coder->id, msg);
	pthread_mutex_unlock(&coder->sim->print_lock);
}

int	is_stopped(t_sim *sim)
{
	int	stopped;

	pthread_mutex_lock(&sim->lock);
	stopped = sim->stop;
	pthread_mutex_unlock(&sim->lock);
	return (stopped);
}

/* sleeps for duration_ms, but wakes up early if the simulation stops */
void	sleep_ms(long duration_ms, t_sim *sim)
{
	long	start;

	start = get_time_in_ms();
	while (get_time_in_ms() - start < duration_ms && !is_stopped(sim))
		usleep(500);
}
