#include "codexion.h"
static int	is_number(char *str)
{
	int	i;

	if (!str || str[0] == '\0')
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}
static int	handle_scheduler(char *str, t_args *args)
{
	if (strcmp(str, "fifo") == 0)
		args->scheduler = 0;
	else if (strcmp(str, "edf") == 0)
		args->scheduler = 1;
	else
	{
		printf("Error: scheduler must be fifo or edf\n");
		return (1);
	}
	return (0);
}
int	parse_args(int ac, char **av, t_args *args)
{
	int	i;

	if (ac != 9)
	return (printf("Error: invalid number of arguments\n"), 1);
	i = 1;
	while (i <= 7)
	{
		if (!is_number(av[i]))
		return (printf("Error: invalid numeric argument\n"), 1);
		i++;
	}
	args->number_of_coders = atoi(av[1]);
	args->time_to_burnout = atoi(av[2]);
	args->time_to_compile = atoi(av[3]);
	args->time_to_debug = atoi(av[4]);
	args->time_to_refactor = atoi(av[5]);
	args->number_of_compiles_required = atoi(av[6]);
	
	args->dongle_cooldown = atoi(av[7]);
	if (handle_scheduler(av[8], args) == 1)
		return (1);

	return (0);
}
int	validate_values(t_args *args)
{
	
	if (args->number_of_coders <= 0)
	{
		printf("Error: invalid number_of_coders\n");
		return (1);
	}
	if (args->time_to_burnout <= 0
		|| args->time_to_compile <= 0
		|| args->time_to_debug <= 0
		|| args->time_to_refactor <= 0)
	{
		printf("Error: invalid time values\n");
		return (1);
	}
	if (args->number_of_compiles_required <= 0)
	{
		printf("Error: invalid compile count\n");
		return (1);
	}
	if (args->dongle_cooldown < 0)
	{
		printf("Error: invalid dongle cooldown\n");
		return (1);
	}
	return (0);
}