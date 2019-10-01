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
		if((pcb[itsr].wake_time == sys_time_count) && (pcb[itsr].state == SLEEP)) {
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
		case SYS_GET_RAND:
			// ?=? just do this directly;
			? = sys_get_rand();
			break;
		case SYS_LOCK_MUTEX:
			SysLockMutex();
			break;
		case SYS_UNLOCK_MUTEX:
			SysUnlockMutex();
			break;
		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();	
	}
		
	if(run_pid!=NONE) {	// if run_pid is not NONE, we penalize it by
		pcb[run_pid].state = READY;	// a. downgrade its state to READY
		EnQue(&ready_que, run_pid);	// b. moving it to the back of the ready-to-run process queue
		run_pid = NONE;	// c. reset run_pid (is now NONE)
	}
	
}


void SysSleep(void) {
	
	int sleep_sec = pcb[run_pid].tf_p->ebx;
	pcb[run_pid].wake_time = (sys_time_count + (sleep_sec * 10));	// p4, now 10 to speedup
	pcb[run_pid].state = SLEEP;	// SLEEP the running process
	run_pid = NONE;	// No running process
	
}


void SysWrite(void) {

	char *str = (char *)pcb[run_pid].tf_p->ebx;	// passed over by a reg val w/i the tf (typecast ebx ?addr? to str to print?)
	
    while( *str != (char) 0 ) {	//show the str one char at a time (use a loop)
		*sys_cursor++ = (*str++)+VGA_MASK_VAL;	// onto console @ sys_cursor position
		if(sys_cursor >= VIDEO_END) sys_cursor = VIDEO_START;	// wrap back to top left
	}
	
}


void SysLockMutex(void) {
	
	int mutex_id;
	
	mutex_id = ...
	
	if(mutex_id == ...) {
		if the lock of the ... is UNLOCKED
			set the lock of the mutex to be LOCKED
		} else {
			suspend the running/calling process: steps 1, 2, 3
		}
	} else {
		cons_printf("Panic: no such mutex ID!\n");
		breakpoint();
	}
	
}


void SysUnlockMutex(void) {
	
	int mutex_id, released_pid;

	mutex_id = ...

	if(mutex_id == ...) {
		if(the suspend queue of the mutex is NOT empty) {
		release the 1st process in the suspend queue: steps 1, 2, 3
		} else {
		 set the lock of the mutex to be UNLOCKED
		}
	} else {
		cons_printf("Panic: no such mutex ID!\n");
		breakpoint();
	}
	
}

		

void SysSetCursor(void) { sys_cursor = VIDEO_START + pcb[run_pid].tf_p->ebx; /* Offset in ebx */ }


void SysFork(void) {

	// 1. allocate a new PID and add it to ready_que (similar to start of SpawnSR)
	int pidF, distance, *bpEbp, *ret;
	pidF = DeQue(&avail_que);
	Bzero((char *)&pcb[pidF], sizeof(pcb_t));
	EnQue(&ready_que, pidF);

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
	pcb[pidF].tf_p = (tf_t*)(distance + (int)pcb[run_pid].tf_p);
	
	// 6. use child's trapframe pointer to adjust these in the trapframe:
	// eip (so it points o child's own instructions),
	// ebp (so it points to child's local data),
	//cons_printf("parent of fork run_pid %d eip%d ebp%d\n", run_pid, pcb[run_pid].tf_p->eip, pcb[run_pid].tf_p->ebp);
	pcb[pidF].tf_p->eip = (distance + (int)pcb[run_pid].tf_p->eip);
	pcb[pidF].tf_p->ebp = (distance + (int)pcb[run_pid].tf_p->ebp);
	
	// also, the value where ebp points to:
	// treat ebp as an integer pointer and alter what it points to (chain of bp)
	
	// now, to deal with the Init process' main bp, since we have been manipulating the called syscallFork's "sub" bp
	bpEbp = (int*) (pcb[pidF].tf_p->ebp);	// var recieves ebp as int pointer
	*bpEbp += distance;	// offset the value pointed to by pointer by value 'distance'
	bpEbp = (int *)*bpEbp;	// convert that into a pointer
	// that took all the way until ptr5.c in your "PtrBasics" tutorial
	
	// also return address (from in class on Sept30) - otherwise, the eip will just jump back to parent location
	ret = (int*) (pcb[pidF].tf_p->ebp+4);
	*ret += distance;
	ret = (int *) *ret;
	
	// 7. correctly set return values of sys_fork():
	pcb[run_pid].tf_p->ebx = pidF;	// ebx in the parent's trapframe gets the new child PID
	pcb[pidF].tf_p->ebx = 0;	// ebx in the child's trapframe gets ? (is it 0?)
	
}