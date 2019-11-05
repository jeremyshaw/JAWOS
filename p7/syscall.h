// syscall.h
// JAWOS Jeremy Alex Will OS
#ifndef _SYSCALL_ //following naming convention from other files
#define _SYSCALL_

#include "const-type.h"     // for SYS_GET_PID, etc., below

int sys_get_pid(void);
int sys_get_time(void);
void sys_sleep(int sleep_sec);
void sys_write(char *str);
void sys_set_cursor(int row, int col);
int sys_fork(void);
unsigned sys_get_rand(void);
void sys_lock_mutex(int mutex_id);
void sys_unlock_mutex(int mutex_id);
void sys_exit(int exit_code);
int sys_wait(int *exit_code);
void sys_signal(int signal_name, func_p_t p);
void sys_kill(int pid, int signal_name);

void sys_read(char *str);

#endif
