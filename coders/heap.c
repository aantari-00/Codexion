/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aantari <aantari@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 20:44:52 by aantari           #+#    #+#             */
/*   Updated: 2026/08/24 20:44:53 by aantari          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	heap_destroy(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

void	heap_push(t_heap *heap, t_request req, int scheduler)
{
	int	i;

	if (heap->size >= heap->capacity)
		return ;
	i = heap->size;
	heap->items[i] = req;
	heap->size++;
	sift_up(heap, i, scheduler);
}

t_coder	*heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (NULL);
	return (heap->items[0].coder);
}

void	heap_pop(t_heap *heap, int scheduler)
{
	if (heap->size == 0)
		return ;
	heap->size--;
	heap->items[0] = heap->items[heap->size];
	sift_down(heap, 0, scheduler);
}
