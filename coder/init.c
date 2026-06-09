#include "codexion.h"

static	int	init_memory(t_sim *sim)
{
	int	n;

	n = sim->args.number_of_coders;
	sim->coders = malloc(sizeof(t_coders) * n);
	sim->dongles = malloc(sizeof(t_dongle) * n);
	if (!sim->coders || !sim->dongles)
		return (1);
	return (0);
}
static int init_dongles(t_sim *sim)
{
    long    n, i;
    
    i = 0;
    n = sim->args.number_of_coders;
    while(i < n)
    {
        if(pthread_mutex_init(&sim->dongles[i].dongle_mutex, NULL) != 0)
            return (1);
        sim->dongles[i].last_used_time = 0;
        sim->dongles[i].in_use = 0;
        i++;
    }
    return (0);
}
static int init_coders(t_sim *sim)
{
    long    n, i;

    i = 0;
    n = sim->args.number_of_coders;
    while(i < n)
    {
        sim->coders[i].coder_id = i + 1;
        sim->coders[i].compile_count = 0;
        sim->coders[i].last_compile_start = sim->start_time;
		sim->coders[i].left_dongle = &sim->dongles[i];
		sim->coders[i].right_dongle = &sim->dongles[(i + 1) % n];
        sim->coders[i].sim = sim;
        i++;
    }
    return (0);
}
static int init_mutex(t_sim *sim)
{
    sim->stop = 0;
    if(pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
        return(1);
    if(pthread_mutex_init(&sim->print_mutex, NULL) != 0)
        return(1);
    return (0);
    
}
int	init_sim(t_sim *sim, t_args args)
{
	sim->args = args;
	sim->start_time = 0;
    sim->stop = 0;
	if (init_memory(sim))
		return (1);
	if (init_mutex(sim))
		return (1);
	if (init_dongles(sim))
		return (1);
	if (init_coders(sim))
		return (1);
	return (0);
}