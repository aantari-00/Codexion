#include "codexion.h"

long get_time_in_ms(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void smart_sleep(long time_in_ms, t_sim *sim)
{
    long start_time;

    start_time = get_time_in_ms();
    while ((get_time_in_ms() - start_time) < time_in_ms && sim->stop == 0)
        usleep(500);
}
void print_status(t_coders *coder, char *msg)
{
    long time;

    pthread_mutex_lock(&coder->sim->stop_mutex);
    if (coder->sim->stop == 1)
    {
        pthread_mutex_unlock(&coder->sim->stop_mutex);
        return;
    }
    
    pthread_mutex_unlock(&coder->sim->stop_mutex);

    pthread_mutex_lock(&coder->sim->print_mutex);
    time = get_time_in_ms() - coder->sim->start_time;
    printf("%ld %d %s\n", time, coder->coder_id, msg);
    pthread_mutex_unlock(&coder->sim->print_mutex);
}