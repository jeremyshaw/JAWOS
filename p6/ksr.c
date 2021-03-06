// ksr.c, 159 Kernal Service Routines
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "syscall.h"
#include "proc.h"	// remove this if uneccsary, only use for Init in SysExit AlterStack call


void SpawnSR(func_p_t p) {
	
	int pid;
	
	if(QueEmpty(&avail_que)){
		cons_printf("Panic: out of PID!\n");
		breakpoint();
	}
	
	pid = DeQue(&avail_que);
	Bzero((char *)&pcb[pid], sizeof(pcb_t));
	pcb[pid].state = READY;
	if(pid != IDLE) EnQue(&ready_que, pid);
	
	MemCpy( (char *) (DRAM_START + ( pid * STACK_MAX )), (char *)p, STACK_MAX );
	
	// point tf_p to stack & fill TF out
	pcb[pid].tf_p = (tf_t *)(DRAM_START + ( (pid+1)*STACK_MAX - sizeof(tf_t) ));
	pcb[pid].tf_p -> efl = EF_DEFAULT_VALUE|EF_INTR; //handle intr
	pcb[pid].tf_p -> cs = get_cs();
	pcb[pid].tf_p -> eip = (DRAM_START + (pid*STACK_MAX));
	
}


void TimerSR(void) {
	
	int itsr;
	
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
    
	sys_time_count++;
	pcb[run_pid].time_count++;
   	pcb[run_pid].total_time++;
	
	for(itsr = 0; itsr < PROC_MAX; itsr++) {
		if((pcb[itsr].wake_time == sys_time_count) && (pcb[itsr].state == SLEEP)) {
			pcb[itsr].state = READY;
			EnQue(&ready_que, itsr);
		}
	}

	if(run_pid == IDLE) return;

	if(pcb[run_pid].time_count == TIME_MAX){
		EnQue(&ready_que, run_pid);
		pcb[run_pid].state = READY;
		run_pid = NONE;
    }
	
}


void SyscallSR(void) {

	switch (pcb[run_pid].tf_p->eax) {
		case SYS_GET_PID:
			pcb[run_pid].tf_p->ebx = run_pid;
			break;
		case SYS_GET_TIME:
			pcb[run_pid].tf_p->ebx = sys_time_count;
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
		case SYS_SIGNAL:
			SysSignal();
			break;
		case SYS_KILL:
			SysKill();
			break;
		default:
			cons_printf("Kernel Panic: no such syscall!\n");
			breakpoint();	
	}
	
	if(run_pid != NONE) {	// basically, any syscall causes a demotion into a RR
		pcb[run_pid].state = READY;
		EnQue(&ready_que, run_pid);
		run_pid = NONE;
	}
	
}


void SysSignal(void){
	// use sig name (as array index) & funct ptr (as value) to initialize the sig-hndlr arry in run_pid PCB
	pcb[run_pid].signal_handler[pcb[run_pid].tf_p->ebx] = (func_p_t)pcb[run_pid].tf_p->edx;
}


void SysKill(void){
	int pid, signal_name, i;
	pid = pcb[run_pid].tf_p->edx;
	signal_name = pcb[run_pid].tf_p->ebx;
	if(pid == 0 && signal_name == SIGCONT){	// if pid=0 and sig=SIGCONT: 
		for(i = 0; i<PROC_MAX; i++) {	// wake up sleeping children of run_pid
			if(pcb[i].ppid == run_pid && pcb[i].state == SLEEP){
				pcb[i].state = READY;
				EnQue(&ready_que, i);
			}
		}
	}
}


void AlterStack(int pid, func_p_t p){
	
	int *local;
	unsigned eip;
	tf_t tmp;

	tmp = *pcb[pid].tf_p;	// deref tf_p (to get data inside trapframe), copy to temp trapframe; tmp = *pcb[pid].tf_p
	eip = pcb[pid].tf_p->eip;		
	local = &pcb[pid].tf_p->efl;	// efl is at top of stack, address is where we insert later
	tmp.eip = (unsigned int)p;	// tmp.eip = handler addr
	pcb[pid].tf_p = (tf_t*)((int)pcb[pid].tf_p - 4);	// then decrease tf_p by 4 points (change to int, minus 4, then change back)
	*pcb[pid].tf_p = tmp;	// *pcb...tf_p = tmp (don't need memcpy)
	*local = eip;
		
}

void SysExit(void) {	
	
	int ppid;
	ppid = pcb[run_pid].ppid;
	
	if(pcb[ppid].state == WAIT) {
		pcb[ppid].state = READY;
		EnQue(&ready_que, ppid);
		pcb[ppid].tf_p->edx = run_pid;
		*((int *)pcb[ppid].tf_p->ebx) = pcb[run_pid].tf_p->ebx;
		pcb[run_pid].state = AVAIL;
		EnQue(&avail_que, run_pid);
	} else { 
		pcb[run_pid].state = ZOMBIE; 
		if(pcb[ppid].signal_handler[SIGCHLD] != 0) {	// check if parent has 'registered' a handler for SIGCHLD event 
			AlterStack(ppid, pcb[ppid].signal_handler[SIGCHLD]);	// from AlterPid, we are passing in the handler func_ptr
		}
	} 
	run_pid = NONE;
}


void SysWait(void) {
	
	int i;	
	for(i = 0; i < PROC_MAX; i++) {
		if(pcb[i].state == ZOMBIE && pcb[i].ppid == run_pid) break;
	}
	
	if(i == PROC_MAX){
		pcb[run_pid].state = WAIT;
		run_pid = NONE;
	} else {
		pcb[run_pid].tf_p->edx = i;
		*((int *)pcb[run_pid].tf_p->ebx) = pcb[i].tf_p->ebx;
		pcb[i].state = AVAIL;
		EnQue(&avail_que, i);
	}
	
}


void SysSleep(void) {
	
	int sleep_sec = pcb[run_pid].tf_p->ebx;
	pcb[run_pid].wake_time = (sys_time_count + (sleep_sec * 10));
	pcb[run_pid].state = SLEEP;
	run_pid = NONE;
	
}


void SysWrite(void) {

	char *str = (char *)pcb[run_pid].tf_p->ebx;
    while( *str != (char) 0 ) {
		*sys_cursor++ = (*str++)+VGA_MASK_VAL;
		if(sys_cursor >= VIDEO_END) sys_cursor = VIDEO_START;
	}
	
}


void SysLockMutex(void) {
	
	int mutex_id;
	mutex_id = pcb[run_pid].tf_p->ebx;
	
	if(mutex_id == VIDEO_MUTEX) {
		if(video_mutex.lock == UNLOCKED) video_mutex.lock = LOCKED;
		else {
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
		if(QueEmpty(&video_mutex.suspend_que) != 1) {
			released_pid = DeQue(&video_mutex.suspend_que);
			pcb[released_pid].state = READY;
			EnQue(&ready_que, released_pid);
		} else video_mutex.lock = UNLOCKED;
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

	if(QueEmpty(&avail_que)) pcb[run_pid].tf_p->ebx = NONE;
	else {
		pidF = DeQue(&avail_que);
		EnQue(&ready_que, pidF);

		MemCpy((char*)&pcb[pidF], (char*)&pcb[run_pid], sizeof(pcb_t));
		pcb[pidF].state = READY;
		pcb[pidF].time_count = 0;
		pcb[pidF].total_time = 0;
		pcb[pidF].ppid = run_pid;

		MemCpy((char *)(DRAM_START + (pidF * STACK_MAX)), (char *)(DRAM_START + (run_pid * STACK_MAX)), STACK_MAX);
		
		distance = ((pidF-run_pid) * STACK_MAX);
		pcb[pidF].tf_p = (tf_t*)(distance + (int)pcb[run_pid].tf_p);
		pcb[pidF].tf_p->eip = (distance + pcb[run_pid].tf_p->eip);
		pcb[pidF].tf_p->ebp = (distance + pcb[run_pid].tf_p->ebp);
		
		bpEbp = (int*) (pcb[pidF].tf_p->ebp);
		bpEbp[0] += distance;	// for ebp of the caller
		bpEbp[1] += distance;	// handles the iret/eip
		
		pcb[run_pid].tf_p->ebx = pidF;
		pcb[pidF].tf_p->ebx = 0;
	}		
}