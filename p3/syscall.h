// syscall.h
#ifndef _SYSCALL_ //following naming convention from other files
#define _SYSCALL_

#include "const-type.h"     // for SYS_GET_PID, etc., below

int sys_get_pid(void);
int sys_get_time(void);
void sys_sleep(int sleep_sec);
void sys_write(char *str);
void sys_set_cursor(int row, int col);
int sys_fork(void);

#endif
