// ext-data.h, 159
// kernel data are all declared in kernel.c during bootstrap
// .c code, if needed these, include this

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _EXT_DATA_
#define _EXT_DATA_

#include "const-type.h"

extern int run_pid;                 // PID of current selected running process
//and other extern ...
extern int pid;

extern unsigned int sys_time_count;

extern que_t avail_que;
extern que_t ready_que;

extern pcb_t pcb[PROC_MAX];

extern struct i386_gate *idt;   

//extern //... all from main?

//void Kernel(tf_t *tf_p);
//void Scheduler(void);
//void BootStrap(void)
//int main(void);

#endif
