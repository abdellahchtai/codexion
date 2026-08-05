/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abchtaib <abchtaib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:21:22 by abchtaib          #+#    #+#             */
/*   Updated: 2026/08/05 17:06:31 by abchtaib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	min_request(t_request a, t_request b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.tie_breaker < b.tie_breaker);
}

void	heapify_up(t_request *heap, int index)
{
	int	parent;

	while (index)
	{
		parent = (index - 1) / 2;
		if (min_request(heap[index], heap[parent]))
			heap_swap(&heap[index], &heap[parent]);
		else
			break ;
		index = parent;
	}
}

int	heap_push(t_dongle *dongle, t_request request)
{
	pthread_mutex_lock(&dongle->lock);
	if (dongle->heap_size == dongle->heap_capacity)
		return (pthread_mutex_unlock(&dongle->lock), 0);
	dongle->heap[dongle->heap_size] = request;
	dongle->heap_size++;
	heapify_up(dongle->heap, dongle->heap_size - 1);
	return (pthread_mutex_unlock(&dongle->lock), 1);
}

void	heapify_down(t_dongle *dongle)
{
	int	i;
	int	left;
	int	right;
	int	smaller;

	i = 0;
	while (1)
	{
		left = i * 2 + 1;
		right = i * 2 + 2;
		smaller = i;
		if (left < dongle->heap_size && min_request(dongle->heap[left],
				dongle->heap[smaller]))
			smaller = left;
		if (right < dongle->heap_size && min_request(dongle->heap[right],
				dongle->heap[smaller]))
			smaller = right;
		if (smaller == i)
			break ;
		heap_swap(&dongle->heap[i], &dongle->heap[smaller]);
		i = smaller;
	}
}

t_request	heap_pop(t_dongle *dongle)
{
	t_request	request_wanted;

	if (dongle->heap_size == 0)
	{
		request_wanted.key = -1;
		request_wanted.coder_id = -1;
		return (request_wanted);
	}
	request_wanted = dongle->heap[0];
	dongle->heap[0] = dongle->heap[dongle->heap_size - 1];
	dongle->heap_size--;
	heapify_down(dongle);
	return (request_wanted);
}
