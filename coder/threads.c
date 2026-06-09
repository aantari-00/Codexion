#include "codexion.h"
int create_threads(t_sim *sim)
{
    int num , i;

    num = sim->args.number_of_coders;
    i = 0;
    while(i < num)
    {
        pthead_creat(&sim->coders[i].thread, NULL, &routine, &sim->coders[i]);
    }

}
int join_thread(t_sim *sim)
{
    int num , i;

    num = sim->args.number_of_coders;
    i = 0;
    while(i < num)
    {
        pthread_join(sim->coders[i].thread, NULL);
    }
}