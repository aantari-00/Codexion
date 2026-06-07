#ifndef CODEXION_H
#define CODEXION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

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
    pthread_mutex_t	mutex;
    long            last_used_time;
} t_dongle;

//
#endif