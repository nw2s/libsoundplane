#ifndef MEC_H
#define MEC_H

#include <pthread.h>

void *eigenharp_proc(void *);
void *osc_command_proc(void *);
void *soundplane_proc(void *);

extern pthread_cond_t  waitCond;
extern pthread_mutex_t waitMtx;
extern volatile int keepRunning;

#endif
