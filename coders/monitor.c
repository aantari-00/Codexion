#include "codexion.h"

static int	coder_burned_out(t_coder *coder, long now)
{
	return (!coder->finished
		&& now - coder->last_compile_start >= coder->sim->args.time_to_burnout);
}

static int	check_coders(t_sim *sim)
{
	int		i;
	long	now;
	int		all_finished;

	now = get_time_in_ms();
	all_finished = 1;
	i = 0;
	while (i < sim->args.number_of_coders)
	{
		if (coder_burned_out(&sim->coders[i], now))
		{
			print_status(&sim->coders[i], "burned out");
			return (1);
		}
		if (!sim->coders[i].finished)
			all_finished = 0;
		i++;
	}
	return (all_finished);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		should_stop;

	sim = (t_sim *)arg;
	while (1)
	{
		pthread_mutex_lock(&sim->lock);
		should_stop = check_coders(sim);
		if (should_stop)
			sim->stop = 1;
		pthread_cond_broadcast(&sim->wake);
		pthread_mutex_unlock(&sim->lock);
		if (should_stop)
			break ;
		usleep(1000);
	}
	return (NULL);
}

int	create_monitor_thread(t_sim *sim)
{
	if (pthread_create(&sim->monitor_thread, NULL, monitor_routine, sim) != 0)
		return (1);
	return (0);
}

int	join_monitor_thread(t_sim *sim)
{
	if (pthread_join(sim->monitor_thread, NULL) != 0)
		return (1);
	return (0);
}
