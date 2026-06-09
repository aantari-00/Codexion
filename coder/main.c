#include "codexion.h"

int main(int argc, char **argv)
{
    t_sim sim;
    t_args args;

    if(pars_arg(argc, argv, &args) != 0)
        return (1);
    if(validate_values(&args) != 0)
        return (1);
    if(init_sim(&sim, &args) != 0)
        return (1);
    if(create_threads(&sim) != 0)
        return (1);
    if(join_threads(&sim) != 0)
        return (1);
    
}
