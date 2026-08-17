#include "codexion.h"

/*
** Should request "a" be served before request "b"?
** - fifo: whoever asked first (smaller req_time) goes first.
** - edf : whoever has the closest burnout deadline goes first,
**         ties broken by whoever asked first.
*/
static int	is_higher_priority(t_request *a, t_request *b, int scheduler)
{
	if (scheduler == SCHEDULER_EDF && a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->req_time < b->req_time);
}

static void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	sift_up(t_heap *heap, int i, int scheduler)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!is_higher_priority(&heap->items[i], &heap->items[parent], scheduler))
			break ;
		swap_requests(&heap->items[i], &heap->items[parent]);
		i = parent;
	}
}

static void	sift_down(t_heap *heap, int i, int scheduler)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = i * 2 + 1;
		right = i * 2 + 2;
		smallest = i;
		if (left < heap->size && is_higher_priority(&heap->items[left],
				&heap->items[smallest], scheduler))
			smallest = left;
		if (right < heap->size && is_higher_priority(&heap->items[right],
				&heap->items[smallest], scheduler))
			smallest = right;
		if (smallest == i)
			break ;
		swap_requests(&heap->items[i], &heap->items[smallest]);
		i = smallest;
	}
}

int	heap_init(t_heap *heap, int capacity)
{
	heap->items = malloc(sizeof(t_request) * capacity);
	if (!heap->items)
		return (1);
	heap->size = 0;
	heap->capacity = capacity;
	return (0);
}

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
