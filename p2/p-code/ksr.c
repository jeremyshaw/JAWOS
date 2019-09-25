// ksr.c, 159 Kernal Service Routines
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"

int itsr;

void SpawnSR(func_p_t p) {     // arg: where process code starts
	
	int pid;
	
	if(QueEmpty(&avail_que)==1){
		cons_printf("Panic: out of PID!\n");
		breakpoint();
	}
	
	pid = DeQue(&avail_que);
	Bzero((char *)&pcb[pid], sizeof(pcb_t));
	pcb[pid].state = READY;
	if(pid != IDLE) EnQue(&ready_que, pid);
	
	// copy code to DRAM, both code & stack are separated among processes, phase2
	MemCpy( (char *) (DRAM_START + ( pid * STACK_MAX )), (char *)p, STACK_MAX );
	
	// point tf_p to stack & fill TF out
	pcb[pid].tf_p = (tf_t *)(DRAM_START + ( (pid+1)*STACK_MAX - sizeof(tf_t) ));
	pcb[pid].tf_p -> efl = EF_DEFAULT_VALUE|EF_INTR; //handle intr
	pcb[pid].tf_p -> cs = get_cs();
	pcb[pid].tf_p -> eip = (DRAM_START + (pid*STACK_MAX));
	

}


void TimerSR(void) {	// count run time and switch if hitting time limit
	
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
    
    sys_time_count++;
	
	pcb[run_pid].time_count++;
   	pcb[run_pid].total_time++;
	
	//Use a loop to look for any processes that need to be waken up!
	for(itsr = 0; itsr < PROC_MAX; itsr++) {
		if(pcb[itsr].wake_time == sys_time_count) {
			pcb[itsr].state = READY;
			EnQue(&ready_que, itsr);
		}
	}

	if(run_pid == IDLE) return;    // Idle exempt from below, phase2

	if(pcb[run_pid].time_count == TIME_MAX){
		EnQue(&ready_que, run_pid);
		pcb[run_pid].state = READY;
		run_pid = NONE;
    }
	
}


void SyscallSR(void) {

	switch (pcb[run_pid].tf_p->eax) {
		case SYS_GET_PID:
			pcb[run_pid].tf_p->ebx = run_pid;	// run_pid to ebx in the tf of the running process
			break;
		case SYS_GET_TIME:
			pcb[run_pid].tf_p->ebx = sys_time_count;	// sys_time_count to ebx in tf of the running process
			break;			
		case SYS_SLEEP:
			SysSleep();
			break;
		case SYS_WRITE:
			SysWrite();
			break;
		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();	
	}
	
}


void SysSleep(void) {
	
	// from reg value w/i tf (ebx?) calc wake_time of the running process 
	// using sys_time_count plus the sleep_sec times 100
	int sleep_sec = pcb[run_pid].tf_p->ebx;
	pcb[run_pid].wake_time = (sys_time_count + (sleep_sec * 100));
	pcb[run_pid].state = SLEEP;	// alter the state of the running process to SLEEP
	run_pid = NONE;	// alter the value of run_pid to NONE
	
}


void SysWrite(void) {

	char *str = (char *)pcb[run_pid].tf_p->ebx;// passed over by a reg val w/i the tf (typecast ebx ?addr? to str to print?)
	
    while( *str != (char) 0 ) {	//show the str one char at a time (use a loop)
		//onto the console (at the system cursor position)
		*sys_cursor++ = (*str++)+VGA_MASK_VAL;
		
		// while doing so, the cursor may wrap back to the top-left corner if needed
		if(sys_cursor >= VIDEO_END) sys_cursor = VIDEO_START;
	}
	
}

