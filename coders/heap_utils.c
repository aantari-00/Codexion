# include	"codexion.h"

void	swap_requests(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	sift_down(t_heap *heap, int i, int scheduler)
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

int	is_higher_priority(t_request *a, t_request *b, int scheduler)
{
	if (scheduler == SCHEDULER_EDF && a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	return (a->req_time < b->req_time);
}

void	sift_up(t_heap *heap, int i, int scheduler)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (!is_higher_priority(&heap->items[i],
				&heap->items[parent], scheduler))
			break ;
		swap_requests(&heap->items[i], &heap->items[parent]);
		i = parent;
	}
}
