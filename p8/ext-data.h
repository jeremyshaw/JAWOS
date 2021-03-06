// ext-data.h, 159
// kernel data are all declared in kernel.c during bootstrap
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _EXT_DATA_
#define _EXT_DATA_

#include "const-type.h"

extern que_t avail_que, ready_que;
extern pcb_t pcb[PROC_MAX];
extern unsigned int sys_time_count, sys_rand_count, KDir;
extern int run_pid, pid;
extern struct i386_gate *idt;   
extern unsigned short * sys_cursor;
extern mutex_t video_mutex;
extern kb_t kb;
extern page_t page[PAGE_MAX];

#endif