/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: safe <safe@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:45:09 by aantari           #+#    #+#             */
/*   Updated: 2026/09/05 02:40:25 by safe             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_number(const char *str)
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

static long	ft_atol(char *str)
{
	int		i;
	long	num;
	int		signe;

	i = 0;
	num = 0;
	signe = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			signe = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		num = num * 10 + (str[i] - 48);
		i++;
	}
	return (num * signe);
}

static int	parse_scheduler(const char *str, t_args *args)
{
	if (strcmp(str, "fifo") == 0)
		args->scheduler = SCHEDULER_FIFO;
	else if (strcmp(str, "edf") == 0)
		args->scheduler = SCHEDULER_EDF;
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
	{
		printf("Error: wrong number of arguments\n");
		printf("Usage: %s number_of_coders time_to_burnout time_to_compile "
			"time_to_debug time_to_refactor number_of_compiles_required "
			"dongle_cooldown scheduler\n", av[0]);
		return (1);
	}
	i = 1;
	while (i <= 7)
	{
		if (!is_number(av[i]))
			return (printf("Error: argument must be a positive integer\n"), 1);
		i++;
	}
	args->number_of_coders = ft_atol(av[1]);
	args->time_to_burnout = ft_atol(av[2]);
	args->time_to_compile = ft_atol(av[3]);
	args->time_to_debug = ft_atol(av[4]);
	args->time_to_refactor = ft_atol(av[5]);
	args->number_of_compiles_required = ft_atol(av[6]);
	args->dongle_cooldown = ft_atol(av[7]);
	return (parse_scheduler(av[8], args));
}

int	validate_args(t_args *args)
{
	if (args->number_of_coders <= 0)
		return (printf("Error: number_of_coders must be greater than 0\n"), 1);
	if (args->time_to_burnout <= 0 || args->time_to_compile <= 0
		|| args->time_to_debug <= 0 || args->time_to_refactor <= 0)
		return (printf("Error: time values must be greater than 0\n"), 1);
	if (args->number_of_compiles_required <= 0)
		return (printf("Error: number_of_compiles_required must be > 0\n"), 1);
	if (args->dongle_cooldown < 0)
		return (printf("Error: dongle_cooldown must not be negative\n"), 1);
	return (0);
}
