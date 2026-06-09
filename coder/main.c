#include "codexion.h"


int main(int ac, char **av)
{
    t_args args;
    t_sim sim;

    if (parse_args(ac, av, &args))
        return (1);
    if (validate_values(&args))
        return (1);
    if (init_sim(&sim, args))
        return (1);
    if (create_threads(&sim))
        return (1);
    if (join_threads(&sim))
        return (1);
    return (0);

}
