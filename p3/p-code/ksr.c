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
		case SYS_FORK:
			SysFork();
			break;
		case SYS_SET_CURSOR:
			SysSetCursor();
			break;
		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();	
	}
	
}


void SysSleep(void) {
	
	// from passed value w/i tf->ebx calc wake_time of the running process 
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


void SysSetCursor(void) { sys_cursor = VIDEO_START + pcb[run_pid].tf_p->ebx; /* Offset in ebx */ }


void SysFork(void) {

	// 1. allocate a new PID and add it to ready_que (similar to start of SpawnSR)
	int pidF, distance, trap, *bpEbp;
	pidF = DeQue(&avail_que);
	Bzero((char *)&pcb[pidF], sizeof(pcb_t));
	EnQue(&ready_que, pidF);
	cons_printf("pidF %d, run_pid %d\n", pidF, run_pid);

	// 2. copy PCB from parent process, but alter these:
	// process state, the two time counts, and ppid
	pcb[pidF].state = READY;
	pcb[pidF].time_count = 0;
	pcb[pidF].total_time = 0;
	pcb[pidF].ppid = run_pid; //gives parent pid to ppid of child

	// 3. copy the process image (the 4KB DRAM) from parent to child:
	// figure out destination and source byte addresses
	// use tool MemCpy() to do the copying
	MemCpy((char *)(DRAM_START + (pidF * STACK_MAX)), (char *)(DRAM_START + (run_pid * STACK_MAX)), STACK_MAX);
	
	// 4. calculate the byte distance between the two processes
	// = (child PID - parent PID) * 4K
	distance = ((pidF-run_pid) * STACK_MAX);

	// 5. apply the distance to the trapframe location in child's PCB
	//cons_printf("pidF %d tf %d run_pid %d tf %d\n", pidF, pcb[pidF].tf_p, run_pid, pcb[run_pid].tf_p);
	trap = distance + (int)pcb[run_pid].tf_p;
	cons_printf("pidF %d trap %d distance %d p[r].t %d\n", pidF, trap, distance, pcb[run_pid].tf_p);
	pcb[pidF].tf_p = (tf_t*)(distance + (int)pcb[run_pid].tf_p);
	
	// 6. use child's trapframe pointer to adjust these in the trapframe:
	// eip (so it points o child's own instructions),
	// ebp (so it points to child's local data),
	// also, the value where ebp points to:
	// treat ebp as an integer pointer and alter what it points to (chain of bp)
	cons_printf("run_pid %d eip%d ebp%d\n", run_pid, pcb[run_pid].tf_p->eip, pcb[run_pid].tf_p->ebp);
	pcb[pidF].tf_p->eip = (distance + (int)pcb[run_pid].tf_p->eip);
	pcb[pidF].tf_p->ebp = (distance + (int)pcb[run_pid].tf_p->ebp);
	
	//from phase 2
	pcb[pidF].tf_p->efl = EF_DEFAULT_VALUE|EF_INTR; //handle intr
	pcb[pidF].tf_p->edi = (distance + (int)pcb[run_pid].tf_p->edi);	// destination index
	pcb[pidF].tf_p->cs = get_cs();
	pcb[pidF].tf_p->eax = 0;	// neutralize the syscall var
	
	// heck, catch all of them.
	pcb[pidF].tf_p->esi = (distance + (int)pcb[run_pid].tf_p->esi);	// source index
	pcb[pidF].tf_p->esp = (distance + (int)pcb[run_pid].tf_p->esp);
	
	// now, to deal with the Init process' main bp, since we have been manipulating the called syscallFork's "sub" bp
	bpEbp = (int*) (pcb[pidF].tf_p->ebp);	// var recieves ebp as int pointer
	*bpEbp += distance;	// offset the value pointed to by pointer by value 'distance'
	bpEbp = (int *)*bpEbp;	// convert that into a pointer
	cons_printf("pidF %d eip%d ebp%d bpEbp %d\n", pidF, pcb[pidF].tf_p->eip, pcb[pidF].tf_p->ebp, bpEbp);
	
	// 7. correctly set return values of sys_fork():
	// ebx in the parent's trapframe gets the new child PID
	// ebx in the child's trapframe gets ? (is it 0?)
	pcb[run_pid].tf_p->ebx = pidF;
	pcb[pidF].tf_p->ebx = 0;	
	
}