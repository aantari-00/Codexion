/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aantari <aantari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:45:12 by aantari           #+#    #+#             */
/*   Updated: 2026/08/24 20:45:13 by aantari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_sim	*sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!is_stopped(sim))
	{
		if (wait_for_turn(coder))
			break ;
		print_status(coder, "has taken a dongle");
		print_status(coder, "has taken a dongle");
		print_status(coder, "is compiling");
		sleep_ms(sim->args.time_to_compile, sim);
		release_turn(coder);
		if (is_stopped(sim) || coder->finished)
			break ;
		print_status(coder, "is debugging");
		sleep_ms(sim->args.time_to_debug, sim);
		if (is_stopped(sim))
			break ;
		print_status(coder, "is refactoring");
		sleep_ms(sim->args.time_to_refactor, sim);
	}
	return (NULL);
}
