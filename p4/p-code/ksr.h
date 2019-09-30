// ksr.h, 159

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _KSR_
#define _KSR_

#include "const-type.h"//for func_p_t

void SpawnSR(func_p_t p);
void TimerSR(void);
void SyscallSR(void);
void SysSleep(void);
void SysWrite(void);
void SysSetCursor(void);
void SysFork(void);

#endif
