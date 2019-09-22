// syscall.c
// system service calls for kernel services

#include "const-type.h"     // for SYS_GET_PID, etc., below

int sys_get_pid(void) {     // phase2
	int pid;

	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
	   : "=g" (pid)         // output from asm()
	   : "g" (SYS_GET_PID)  // input to asm()
	   : "eax", "ebx"       // clobbered registers
	);

	return pid;
}

... sys_get_time(...) {     // similar to sys_get_pid
	...//do we need to return a proper time struct or will a formatted string do?
	...
	...
}

void sys_sleep(int sleep_sec) {  // phase2
	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // sleep seconds
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_SLEEP), "g" (sleep_sec)  // 2 inputs to asm()
	   : "eax", "ebx"            // clobbered registers
	);
}

... sys_write(...) {             // similar to sys_sleep
	...
	...
	...
}

