#include "codexion.h"

void    *routine(void *arg)
{
    t_coders	*coder;

    coder = (t_coders *)arg;
    while (1)
    {
        printf("Coder %d is thinking\n", coder->coder_id);
    }
    return (NULL);
}