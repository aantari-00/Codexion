#include "codexion.h"
int create_threads(t_sim *sim)
{
    int num , i;

    num = sim->args.number_of_coders;
    i = 0;
    while(i < num)
    {
        pthread_create(&sim->coders[i].thread, NULL, &routine, &sim->coders[i]);
        i++;
    }
    return (0);
}

int join_threads(t_sim *sim)
{
    int num , i;

    num = sim->args.number_of_coders;
    i = 0;
    while(i < num)
    {
        pthread_join(sim->coders[i].thread, NULL);
    }
    return (0);
}