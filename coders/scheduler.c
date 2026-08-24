/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aantari <aantari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:45:15 by aantari           #+#    #+#             */
/*   Updated: 2026/08/24 20:45:16 by aantari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_deadline(t_coder *coder)
{
	return (coder->last_compile_start + coder->sim->args.time_to_burnout);
}

static int	is_coders_turn(t_sim *sim, t_coder *coder, long now)
{
	return (heap_peek(&sim->queue) == coder && can_take_dongles(coder, now));
}

int	wait_for_turn(t_coder *coder)
{
	t_sim		*sim;
	t_request	req;
	long		now;

	sim = coder->sim;
	pthread_mutex_lock(&sim->lock);
	req.coder = coder;
	req.req_time = get_time_in_ms();
	req.deadline = get_deadline(coder);
	heap_push(&sim->queue, req, sim->args.scheduler);
	while (!sim->stop && !is_coders_turn(sim, coder, get_time_in_ms()))
		pthread_cond_wait(&sim->wake, &sim->lock);
	if (sim->stop)
	{
		pthread_mutex_unlock(&sim->lock);
		return (1);
	}
	heap_pop(&sim->queue, sim->args.scheduler);
	now = get_time_in_ms();
	take_dongles(coder);
	coder->last_compile_start = now;
	pthread_mutex_unlock(&sim->lock);
	return (0);
}

void	release_turn(t_coder *coder)
{
	t_sim	*sim;
	long	now;

	sim = coder->sim;
	pthread_mutex_lock(&sim->lock);
	now = get_time_in_ms();
	release_dongles(coder, now);
	coder->compiles_done++;
	if (coder->compiles_done >= sim->args.number_of_compiles_required)
		coder->finished = 1;
	pthread_cond_broadcast(&sim->wake);
	pthread_mutex_unlock(&sim->lock);
}
