// ksr.c, 159 Kernal Service Routines
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "syscall.h"


void SpawnSR(func_p_t p) {     // arg: where process code starts
	
	int pid;
	
	if(QueEmpty(&avail_que)){
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
	
	int itsr;
	
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
			pcb[run_pid].tf_p->ebx = sys_rand_count;
			break;
		case SYS_LOCK_MUTEX:
			SysLockMutex();
			break;
		case SYS_UNLOCK_MUTEX:
			SysUnlockMutex();
			break;
		case SYS_EXIT:
			SysExit();
			break;
		case SYS_WAIT:
			SysWait();
			break;
		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();	
	}
		
	// basically, any syscall causes a demotion into a RR
	if(run_pid != NONE) {	// if run_pid is not NONE, we penalize it by
		pcb[run_pid].state = READY;	// a. downgrade its state to READY
		EnQue(&ready_que, run_pid);	// b. moving it to the back of the ready-to-run process queue
		run_pid = NONE;	// c. reset run_pid (is now NONE)
	}
	
}

void SysExit(void) {	
	
	int i;
	i = pcb[run_pid].ppid;
	
	if(pcb[i].state == WAIT) {
		pcb[i].state = READY;	// release parent: upgrade parent's state
		EnQue(&ready_que, pcb[run_pid].ppid);	// and move parent to be ready to run again
        // also: pass over exiting PID to parent (is it in trapframe?) (edx from run_pid)
		pcb[i].tf_p->edx = run_pid;
        // pass over exit code to parent deref ebx to get ec  (*ebx from run_pid) notes backwards
		*((int *)pcb[i].tf_p->ebx) = *(&pcb[run_pid].tf_p->ebx);	// ebx is e_c

		// also: reclaim child resources; no running process anymore
		pcb[run_pid].state = AVAIL;
		EnQue(&avail_que, run_pid);
	} else {
		pcb[run_pid].state = ZOMBIE;
	} 
	run_pid = NONE;	// both of the cases do this, so may as well move it out
	
}


void SysWait(void) {
	
	int i;	// edx pid, ebx *ec
	// ebx has int * ec; *p = exit_code (from zombie child ebx); first find 1 ZOMBIE, then do the earlier
	// one ebx is from parent, which wants the pointer. The other ebx is from the child, which has the address the pointer wants
	// parent has no penalty in here
	for(i = 0; i < PROC_MAX; i++) {	// any child (of mine?) called to exit?
		if(pcb[i].state == ZOMBIE && pcb[i].ppid == run_pid) break;
	}
	
	if(i == PROC_MAX){	// no child called to exit
		// cons_printf("Wrp%d ",run_pid);
		pcb[run_pid].state = WAIT;	// parent is blocked into WAIT state
		run_pid = NONE;	// no running process anymore
	} else {	// child called to exit
		pcb[run_pid].tf_p->edx = i;	// pass over its PID to parent
		*((int *)pcb[run_pid].tf_p->ebx) = *(&pcb[i].tf_p->ebx);	// pass over its exit code to parent
		pcb[i].state = AVAIL;	// reclaim child resources
		EnQue(&avail_que, i);
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
	mutex_id = pcb[run_pid].tf_p->ebx;
	
	if(mutex_id == VIDEO_MUTEX) {
		if(video_mutex.lock == UNLOCKED)	// if the lock of the ... is UNLOCKED
			video_mutex.lock = LOCKED;	// set the lock of the mutex to be LOCKED
		else {	// suspend the running/calling process: steps 1, 2, 3
			EnQue(&video_mutex.suspend_que, run_pid);
			pcb[run_pid].state = SUSPEND;
			run_pid = NONE;
		}
	} else {
		cons_printf("Panic: no such mutex ID!\n");
		breakpoint();
	}
	
}


void SysUnlockMutex(void) {
	
	int mutex_id, released_pid;
	mutex_id = pcb[run_pid].tf_p->ebx;

	if(mutex_id == VIDEO_MUTEX) {
		if(QueEmpty(&video_mutex.suspend_que) != 1) {	// if suspend_que of the mutex is NOT empty
			// release the 1st process in suspend_que: steps 1, 2, 3
			released_pid = DeQue(&video_mutex.suspend_que);
			pcb[released_pid].state = READY;
			EnQue(&ready_que, released_pid);
		} else video_mutex.lock = UNLOCKED;	// set the lock of the mutex to be UNLOCKED
	} else {
		cons_printf("Panic: no such mutex ID!\n");
		breakpoint();
	}
	
}


void SysSetCursor(void) { 
	
	int row, col;
	row = pcb[run_pid].tf_p->ebx;
	col = pcb[run_pid].tf_p->edx;
	sys_cursor = VIDEO_START + (row * 80) + col; 
	
}


void SysFork(void) {

	int pidF, distance, *bpEbp;
	// 1. When running out of PID, SysFork service should place NONE
   // to ebx in trapframe of process for sys_fork call to return.
	if(QueEmpty(&avail_que)){
		pcb[run_pid].tf_p->ebx = NONE;
	} else {
		pidF = DeQue(&avail_que);
		EnQue(&ready_que, pidF);

		// 2. copy PCB from parent process, but alter these:
		// process state, the two time counts, and ppid
		MemCpy((char*)&pcb[pidF], (char*)&pcb[run_pid], sizeof(pcb_t));
		pcb[pidF].state = READY;
		pcb[pidF].time_count = 0;
		pcb[pidF].total_time = 0;
		pcb[pidF].ppid = run_pid; // child gets parent's pid

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
		pcb[pidF].tf_p->eip = (distance + pcb[run_pid].tf_p->eip);
		pcb[pidF].tf_p->ebp = (distance + pcb[run_pid].tf_p->ebp);
		
		// also, the value where ebp points to:
		// treat ebp as an integer pointer and alter what it points to (chain of bp)
		
		// now, to deal with the Init process' main bp, since we have been manipulating the called syscallFork's "sub" bp
		bpEbp = (int*) (pcb[pidF].tf_p->ebp);
		bpEbp[0] += distance;	// for ebp of the caller
		bpEbp[1] += distance;	// handles the iret/eip
		
		// 7. correctly set return values of sys_fork():
		pcb[run_pid].tf_p->ebx = pidF;	// ebx in the parent's trapframe gets the new child PID
		pcb[pidF].tf_p->ebx = 0;	// ebx in the child's trapframe gets ? (is it 0?)
		// cons_printf("pidF %d t_c %d t_t %d ebx %d ppid %d\n", pidF, pcb[pidF].time_count, pcb[pidF].total_time, pcb[pidF].tf_p->ebx, pcb[pidF].ppid);
	}		
}