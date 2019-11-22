// syscall.c JAWOS 159
// system service calls for kernel services

#include "syscall.h"     // for SYS_GET_PID, etc., below
#include "tools.h"	// for sys_read's DeQue and QueEmpty
#include "ext-data.h"	// for sys_read's kb


int sys_fork(void) {
	
	int fork;

	asm("movl %1, %%eax;     // # for kernel to identify service
		int $128;           // interrupt!
		movl %%ebx, %0"     // after, copy ebx to return
	   : "=g" (fork)         // output from asm()
	   : "g" (SYS_FORK)  // input to asm()
	   : "eax", "ebx"       // clobbered registers
	);

	return fork;
	
}


int sys_get_pid(void) {

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


void sys_read(char *str) {	//142

	char ch;
	char chSt[2];
	int index = 0;
	chSt[1] = '\0';
	while(index != STR_MAX-1) {
		/*
		asm("movl %1, %%eax;
			int $128;
			movl %%ebx, %0"
			:"=g" (ch)
			:"g"(SYS_READ)
			:"eax", "ebx"
		);
		*/
		while (QueEmpty(&kb.buffer)) sys_sleep(1);
		ch = DeQue(&kb.buffer);
		chSt[0] = (char)ch;
		sys_write(chSt);
		if(ch == '\r') break;
		*(str++) = ch;
		index++;
	}
	*str = '\0';
	return;

}


void sys_signal(int signal_name, func_p_t p){	// 140

	asm("movl %0, %%eax;          // # for kernel to identify service
	movl %1, %%ebx;
	movl %2, %%edx;
	int $128"                // interrupt!
	:	// no output from asm()
	: "g" (SYS_SIGNAL), "g" (signal_name), "g" (p)	// 3 inputs to asm()
	: "eax", "ebx", "edx"	// clobbered registers
	);
	
}


void sys_vfork(func_p_t p) {	// 143
	
	asm("movl %0, %%eax;
	movl %1, %%ebx;	// # for kernel to identify service
	int $128"	// after, copy ebx to return
	: 
	: "g" (SYS_VFORK), "g" (p)	// input to asm()
	: "eax", "ebx"	// clobbered registers
	);

}


void sys_kill(int signal_name, int pid){	// 141	

	asm("movl %0, %%eax;          // # for kernel to identify service
	movl %1, %%ebx;
	movl %2, %%edx;
	int $128"                // interrupt!
	:	// no output from asm()
	: "g" (SYS_KILL), "g" (signal_name), "g" (pid)	// 3 inputs to asm()
	: "eax", "ebx", "edx"	// clobbered registers
	);
	
}


void sys_write(char *str) {

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


void sys_set_cursor(int row, int col) {
	
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


int sys_wait(int *exit_code_ptr) {	// 139

	int pid;	// ec to ebx, pid to edx

	asm("movl %1, %%eax;     // # for kernel to identify service
		movl %2, %%ebx;
		int $128;           // interrupt!
		movl %%edx, %0"     // after, copy edx to return
	   : "=g" (pid)         // output from asm()
	   : "g" (SYS_WAIT), "g" (exit_code_ptr)  // input to asm()
	   : "edx", "eax", "ebx"    // clobbered registers
	);
	return pid;
	
}
