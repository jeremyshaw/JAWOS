// syscall.h

#include "const-type.h"     // for SYS_GET_PID, etc., below

int sys_get_pid(void);
int sys_get_time(void);
void sys_sleep(int sleep_sec);
void sys_write(char *str);

