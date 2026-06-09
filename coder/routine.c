#include "codexion.h"

void    *routine(void *arg)
{
    t_coders	*coder;
    t_sim		*sim;

    coder = (t_coders *)arg;
    sim = coder->sim;
    while (1)
    {
        printf("Coder %ld is thinking\n", coder->coder_id);
    }
    return (NULL);
}