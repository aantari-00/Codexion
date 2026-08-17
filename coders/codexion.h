/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <pthread.h>
# include <unistd.h>
# include <sys/time.h>

# define SCHEDULER_FIFO 0
# define SCHEDULER_EDF 1

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;

/*
** One coder waiting for its two dongles. Stored in the scheduler's
** priority queue (see heap.c) while the coder is waiting for its turn.
*/
typedef struct s_request
{
	t_coder	*coder;
	long	req_time;	/* used to order fifo requests, and as an edf tiebreaker */
	long	deadline;	/* used to order edf requests: last_compile_start + burnout */
}	t_request;

/*
** Array based binary min-heap. It stores pending dongle requests and
** always keeps the highest priority one at index 0.
*/
typedef struct s_heap
{
	t_request	*items;
	int			size;
	int			capacity;
}	t_heap;

/* one usb dongle, shared by two neighbouring coders */
typedef struct s_dongle
{
	int		holder;			/* id of the coder holding it, or -1 if free */
	long	available_time;	/* the dongle can be taken again after this time */
}	t_dongle;

typedef struct s_args
{
	int		number_of_coders;
	long	time_to_burnout;
	long	time_to_compile;
	long	time_to_debug;
	long	time_to_refactor;
	int		number_of_compiles_required;
	long	dongle_cooldown;
	int		scheduler;
}	t_args;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	int			left_dongle;
	int			right_dongle;
	long		last_compile_start;
	int			compiles_done;
	int			finished;
	t_sim		*sim;
}	t_coder;

/*
** Everything shared between threads. A single lock protects the
** dongles, the coders' state and the scheduling queue: this keeps the
** synchronization easy to reason about and makes hold-and-wait
** deadlocks impossible, since a coder never holds one dongle while
** waiting for the other (see README.md).
*/
typedef struct s_sim
{
	t_args			args;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		monitor_thread;

	long			start_time;
	int				stop;

	pthread_mutex_t	lock;
	pthread_cond_t	wake;
	pthread_mutex_t	print_lock;
	t_heap			queue;
}	t_sim;

/* parsing.c */
int			parse_args(int ac, char **av, t_args *args);
int			validate_args(t_args *args);

/* init.c */
int			init_sim(t_sim *sim, t_args args);

/* cleanup.c */
void		cleanup_sim(t_sim *sim);

/* threads.c */
int			create_coder_threads(t_sim *sim);
int			join_coder_threads(t_sim *sim);

/* routine.c */
void		*coder_routine(void *arg);

/* dongles.c */
int			can_take_dongles(t_coder *coder, long now);
void		take_dongles(t_coder *coder);
void		release_dongles(t_coder *coder, long now);

/* scheduler.c */
long		get_deadline(t_coder *coder);
int			wait_for_turn(t_coder *coder);
void		release_turn(t_coder *coder);

/* monitor.c */
void		*monitor_routine(void *arg);
int			create_monitor_thread(t_sim *sim);
int			join_monitor_thread(t_sim *sim);

/* heap.c */
int			heap_init(t_heap *heap, int capacity);
void		heap_destroy(t_heap *heap);
void		heap_push(t_heap *heap, t_request req, int scheduler);
t_coder		*heap_peek(t_heap *heap);
void		heap_pop(t_heap *heap, int scheduler);

/* utils.c */
long		get_time_in_ms(void);
void		print_status(t_coder *coder, const char *msg);
int			is_stopped(t_sim *sim);
void		sleep_ms(long duration_ms, t_sim *sim);

#endif
