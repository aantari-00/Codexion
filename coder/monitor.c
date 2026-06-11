#include "codexion.h"

void *monitor(void *arg)
{
    t_sim *sim;
    int i;
    long now;

    sim = (t_sim *)arg;
    while(i < sim->args.number_of_coders)

}
int create_monitor_thread(t_sim *sim)
{
    if(pthread_create(&sim->monitor_thread, NULL, monitor, sim) != 0)
        return (1);
    return (0);
}
int join_monitor_thread(t_sim *sim)
{
    if(pthread_join(sim->monitor_thread, NULL) != 0)
        return (1);
    return (0);
}