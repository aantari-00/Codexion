#ifndef CODEXION_H
#define CODEXION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct s_sim		t_sim;
// arguments structure
typedef struct s_args
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	int				number_of_compiles_required;
	long			dongle_cooldown;
	int				scheduler;
} t_args;

//dongle structure
typedef struct s_dongle
{
    pthread_mutex_t	dongle_mutex;
	int             in_use;
    long            last_used_time;
} t_dongle;

 // coders structure
 typedef struct s_coders
{
	int				coder_id;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	long			last_compile_start;
	long			compile_count;
	t_sim			*sim;

}	t_coders;

typedef struct s_sim
{
	t_args			args;
	t_coders		*coders; 
	t_dongle		*dongles;
	pthread_t		monitor_thread;

	long			start_time;
	int				stop;

	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	last_compile_start_mutex;

}	t_sim;
//function prototypes
/* parsing.c */
int		parse_args(int ac, char **av, t_args *args);
int		validate_values(t_args *args);
/*routine.c */
void 	*routine(void *arg);
/* threads.c */
int 	create_threads(t_sim *sim);
int		join_threads(t_sim *sim);
/* init.c */
int		init_sim(t_sim *sim, t_args args);
// /* utils.c */
// long	get_time_in_ms(void);
// void 	smart_sleep(long time_in_ms, t_sim *sim);
// void 	print_status(t_coders *coder, char *msg);
// int		is_stopped(t_sim *sim);
// void 	one_coder(t_coders *coder);
// /* monitor.c */
// void 	*monitor(void *arg);
// int 	create_monitor_thread(t_sim *sim);
// int 	join_monitor_thread(t_sim *sim);
#endif