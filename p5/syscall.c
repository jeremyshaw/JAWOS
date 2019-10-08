// syscall.c JAWOS 159
// system service calls for kernel services

#include "syscall.h"     // for SYS_GET_PID, etc., below


int sys_fork(void) {                     // phase3
	
	int fork; //forked pid

	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
	   : "=g" (fork)         // output from asm()
	   : "g" (SYS_FORK)  // input to asm()
	   : "eax", "ebx"       // clobbered registers
	);

	return fork;
}


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


int sys_get_time(void) {     

	int timeINT;
	
	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
		: "=g" (timeINT)         // output from asm()
		: "g" (SYS_GET_TIME)  // input to asm()
		: "eax", "ebx"       // clobbered registers
	);
	
	return timeINT;
	
}


unsigned sys_get_rand(void) {	// 135
	
	unsigned ranNum;
	
	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
		: "=g" (ranNum)         // output from asm()
		: "g" (SYS_GET_RAND)  // input to asm()
		: "eax", "ebx"       // clobbered registers
	);
	
	return ranNum;
}


void sys_write(char *str) {             // similar to sys_sleep

	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // address of str?
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_WRITE), "g" (str)  // 2 inputs to asm()
	   : "eax", "ebx"            // clobbered registers
	);
	
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


void sys_set_cursor(int row, int col) {  // phase3
	
	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // offset?
		movl %2, %%edx;
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_SET_CURSOR), "g" (row), "g" (col)  // 2 inputs to asm()
	   : "eax", "ebx", "edx"            // clobbered registers
	);
	
}


void sys_lock_mutex(int mutex_id) {	 // 136
	
	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // offset?
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_LOCK_MUTEX), "g" (mutex_id)  // 2 inputs to asm()
	   : "eax", "ebx"            // clobbered registers
	);
	
}


void sys_unlock_mutex(int mutex_id) {	// 137
	
	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // offset?
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_UNLOCK_MUTEX), "g" (mutex_id)  // 2 inputs to asm()
	   : "eax", "ebx"            // clobbered registers
	);
	
}


void sys_exit(int exit_code) {	// 138
	
	asm("movl %0, %%eax;          // # for kernel to identify service
		movl %1, %%ebx;          // offset?
		int $128"                // interrupt!
	   :                         // no output from asm()
	   : "g" (SYS_EXIT), "g" (exit_code)  // 2 inputs to asm()
	   : "eax", "ebx"            // clobbered registers
	);
	
}

//I have not finished this whatsoever
int sys_wait(int *exit_code) {	// 139

	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
	   : "=g" (*exit_code)         // output from asm()
	   : "g" (SYS_WAIT)  // input to asm()
	   : "eax", "ebx"       // clobbered registers
	);

	return *exit_code;
}


