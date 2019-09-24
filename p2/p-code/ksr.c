// ksr.c, 159 Kernal Service Routines

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"

void SpawnSR(func_p_t p) {     // arg: where process code starts
	In SpawnSR, make sure the code and stack are being assigned to
	a new 4KB region for Idle and Init: multiply STACK_MAX with the
	'pid' dequeued from the 'avail_que' so a new process will occupy
	a different 4KB of the DRAM.

	int pid;

	if(QueEmpty(&avail_que)==1){
		cons_printf("Panic: out of PID!\n");
		breakpoint();
	}

	pid = DeQue(&avail_que);

	Bzero((char *)&pcb[pid], STACK_MAX);
	pcb[pid].state = READY;

	if(pid != IDLE) EnQue(&ready_que, pid);

	// copy code to DRAM, both code & stack are separated among processes, phase2
	MemCpy((char*)DRAM_START, (char *)p, STACK_MAX);

	// point tf_p to stack & fill TF out
	pcb[pid].tf_p = (tf_t *)(DRAM_START + STACK_MAX - sizeof(tf_t));
	pcb[pid].tf_p -> efl = EF_DEFAULT_VALUE|EF_INTR; //handle intr
	pcb[pid].tf_p -> cs = get_cs();
	pcb[pid].tf_p -> eip = DRAM_START;
}

// count run time and switch if hitting time limit
void TimerSR(void) {
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);//what do we put in source?
    
    sys_time_count++;

	pcb[run_pid].time_count++;
   	pcb[run_pid].total_time++;

	
	Use a loop to look for any processes that need to be waken up!


	if(run_pid == IDLE) return;    // Idle exempt from below, phase2

	if(pcb[run_pid].time_count >= TIME_MAX){
		
		EnQue(&ready_que, run_pid);
		pcb[run_pid].state = READY;
		run_pid = NONE;
		
    }
}

void SyscallSR(void) {
	switch (pcb[run_pid].tf_p->eax) {
		case SYS_GET_PID:
			copy run_pid to ebx in the trapframe of the running process
		 
		case SYS_GET_TIME:
			copy the system time count to ebx in the trapframe of the running process

		case SYS_SLEEP:
			SysSleep()

		case SYS_WRITE:
			SysWrite()

		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();
	}
}

void SysSleep(void) {
	int sleep_sec = ... from a register value wtihin the trapframe (ebx? check again)
	calculate the wake time of the running process using the current system
	time count plus the sleep_sec times 100
	alter the state of the running process to SLEEP
	alter the value of run_pid to NONE
}

void SysWrite(void) {
	char *str =  ... passed over by a register value wtihin the trapframe (may have to typecast ebx ?addr? to str to print?)
	show the str one char at a time (use a loop)
      onto the console (at the system cursor position)
      (while doing so, the cursor may wrap back to the top-left corner if needed)
	  
}

