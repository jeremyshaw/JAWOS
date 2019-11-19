// ksr.c, 159 Kernal Service Routines
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"
#include "ksr.h"
#include "syscall.h"


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
	
	pcb[pid].Dir = KDir; 
	page[pid].pid = pid;
	
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


void KBSR(void) {
	
	// Certain SR's (functions in ksr.c) need to switch MMU to use
	// the process' Dir in order to access its virtual space
	// ExitSR, WaitSR, AlterStack, and KBSR - use set_cr3(something) instead of run_pid?
	
	int pidKB;
	char ch;
	
	if (cons_kbhit()) {
		ch = cons_getchar();
		if(ch == '$') breakpoint();	
		if(QueEmpty(&kb.wait_que)) EnQue(&kb.buffer, (int)ch);
		else {
			set_cr3(pcb[pidKB].Dir);
			pidKB = DeQue(&kb.wait_que);
			pcb[pidKB].state = READY;
			EnQue(&ready_que, pidKB);
			pcb[pidKB].tf_p->ebx = ch;
			set_cr3(pcb[run_pid].Dir);
		}
	}
	return;
	
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
		case SYS_READ:
			SysRead();
			break;
		case SYS_VFORK:
			SysVFork();
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
	
	set_cr3(KDir);
	
}


void SysVFork(void) {
	
	// for the 5 page indices: int Dir, IT, DT, IP, DP
	unsigned int Dir, *KKDir;
	int IT, DT, IP, DP, pidF, i, pageNum, pageIndex[5];

	pageNum = 0;
	if(QueEmpty(&avail_que)) {
		cons_printf("Not enough PID\n");
		breakpoint();		
	} else {
		pidF = DeQue(&avail_que);
		EnQue(&ready_que, pidF);
		
		// copy PCB from parent process but change 5 places:
			// state, ppid, two time counts, and tf_p (see below) [virtual, set to 2G-sizeof tf_t)
		MemCpy((char*)&pcb[pidF], (char*)&pcb[run_pid], sizeof(pcb_t));
		pcb[pidF].state = READY;
		pcb[pidF].time_count = 0;
		pcb[pidF].total_time = 0;
		pcb[pidF].ppid = run_pid;
		
		for (i = 0; i < PAGE_MAX; i++) {	// "allocate" 5 pages
			if(page[i].pid == NONE){
				pageIndex[pageNum] = i;
				pageNum++;
			}
			if(pageNum == 5) break;
		}
			
		if(pageNum < 5) {
			cons_printf("Not enough pages!");
			breakpoint();
		}

		// set the five pages to be occupied by the new pid & clear the content part of the five pages
		for(i = 0; i < 5; i ++) {
			page[pageIndex[i]].pid = pidF;
			Bzero(page[pageIndex[i]].u.content, PAGE_SIZE);
		}
		Dir = pageIndex[0];
		IT = pageIndex[1];
		DT = pageIndex[2];
		IP = pageIndex[3];
		DP = pageIndex[4];
		
		// build Dir page
			// copy the first 16 entries from KDir to Dir
			// set entry 256 to the address of IT page (|-ed w/ the present and R/W flags)
			// set entry 511 to the address of DT page (|-ed w/ the present and R/W flags)
		KKDir = (int *)KDir;
		for (i = 0; i < 16; i++) page[Dir].u.entry[i] = KKDir[i];
		page[Dir].u.entry[256] = (page[IT].u.addr|PRESENT|RW);
		page[Dir].u.entry[511] = (page[DT].u.addr|PRESENT|RW);
		
		// build IT page - set entry 0 to the address of IP page (| w/ the present and RO flags)
		// build DT page - set entry 1023 to the address of DP page (| w/ the present & RW flags)
		page[IT].u.entry[0] = ((page[IP].u.addr)|PRESENT|RO);
		page[DT].u.entry[1023] = ((page[DP].u.addr)|PRESENT|RW);
		
		// build IP - copy instructions to IP (src addr is ebx of TF)
		MemCpy((char *)page[IP].u.addr, (char *)(pcb[run_pid].tf_p->ebx), PAGE_SIZE);
		
		// build DP - make sure 1023 is actually getting the right value	
		page[DP].u.entry[1021] = G1;	// 3rd to last in u.entry[] is eip = G1
		page[DP].u.entry[1022] = get_cs();	// 2nd to last in u.entry[] is cs = get_cs()
		page[DP].u.entry[1023] = EF_DEFAULT_VALUE|EF_INTR;	// the last in u.entry[] is efl, = EF_DEF... (like SpawnSR)
		
		pcb[pidF].Dir = page[Dir].u.addr;	// copy u.addr of Dir page to Dir in PCB of the new process
		pcb[pidF].tf_p = (tf_t*)(G2 - sizeof(tf_t));	// tf_p in PCB of new process = G2 - size_of trapframe
		cons_printf("done  ");
		cons_printf("p[Dir]a = %u  ", page[Dir].u.addr);
		cons_printf("p[Dir].u.entry[256] = %u  ", page[Dir].u.entry[256]);
		cons_printf("p[Dir].u.entry[511] = %u  ", page[Dir].u.entry[511]);
		cons_printf("page[IT].u.entry[0] = %u  ", page[IT].u.entry[0]);
		cons_printf("page[DT].u.entry[1023] = %u  ", page[DT].u.entry[1023]);
		cons_printf("pcb[pidF].tf_p = %u  ", pcb[pidF].tf_p);
		
	}
}

void SysRead(void){

	if(QueEmpty(&kb.buffer)) {
		EnQue(&kb.wait_que, run_pid);
		pcb[run_pid].state = IO_WAIT;
		run_pid = NONE;		
	} else pcb[run_pid].tf_p->ebx = (unsigned int)DeQue(&kb.buffer);
	
}


void SysKill(void){
	int i;
	int pid = pcb[run_pid].tf_p->edx;
	int signal_name = pcb[run_pid].tf_p->ebx;
	if(pid == 0 && signal_name == SIGCONT){	// if pid=0 and sig=SIGCONT: 
		for(i = 0; i<PROC_MAX; i++) {	// wake up sleeping children of run_pid
			if(pcb[i].ppid == run_pid  && pcb[i].state == SLEEP){
				pcb[i].state = READY;
				EnQue(&ready_que, i);
			}
		}
	}
}


void AlterStack(int pid, func_p_t p){
	
	// Certain SR's (functions in ksr.c) need to switch MMU to use
	// the process' Dir in order to access its virtual space
	// ExitSR, WaitSR, AlterStack, and KBSR
	
	int *local;
	unsigned eip;
	tf_t tmp;

	set_cr3((pcb[pid].Dir));
	
	tmp = *pcb[pid].tf_p;
	eip = pcb[pid].tf_p->eip;		
	local = &pcb[pid].tf_p->efl;	// efl is at top of stack, address is where we insert later
	tmp.eip = (unsigned int)p;
	pcb[pid].tf_p = (tf_t*)((int)pcb[pid].tf_p - 4);	// shift down by 4
	*pcb[pid].tf_p = tmp;
	*local = eip;
	
	set_cr3(pcb[run_pid].Dir);
	
}


void SysExit(void) {
	
	// Certain SR's (functions in ksr.c) need to switch MMU to use
	// the process' Dir in order to access its virtual space
	// ExitSR, WaitSR, AlterStack, and KBSR
	
	int ppid;
	int i;
	
	ppid = pcb[run_pid].ppid;
	set_cr3(pcb[ppid].Dir);
	
	if(pcb[ppid].state == WAIT) {
		pcb[ppid].state = READY;
		EnQue(&ready_que, ppid);
		pcb[ppid].tf_p->edx = run_pid;
		*((int *)pcb[ppid].tf_p->ebx) = pcb[run_pid].tf_p->ebx;
		pcb[run_pid].state = AVAIL;
		EnQue(&avail_que, run_pid);
	} else { 
		pcb[run_pid].state = ZOMBIE;
		// if parent doesn't have SIGCHLD handler, add it back!
		if(pcb[ppid].signal_handler[SIGCHLD] != 0) AlterStack(ppid, pcb[ppid].signal_handler[SIGCHLD]);
	} 
	for (i = 0; i < PAGE_MAX ; i++) if(page[i].pid == run_pid) page[i].pid = NONE;
	run_pid = NONE;
	set_cr3(KDir);
}


void SysWait(void) {
	
	// Certain SR's (functions in ksr.c) need to switch MMU to use
	// the process' Dir in order to access its virtual space
	// ExitSR, WaitSR, AlterStack, and KBSR
	
	int i, something;	
	for(i = 0; i < PROC_MAX; i++) { if(pcb[i].state == ZOMBIE && pcb[i].ppid == run_pid) break; }
	
	if(i == PROC_MAX){	// nothing is waiting, nothing to run
		pcb[run_pid].state = WAIT;
		run_pid = NONE;
	} else {
		set_cr3(pcb[run_pid].Dir);
		pcb[run_pid].tf_p->edx = i;
		*((int *)pcb[run_pid].tf_p->ebx) = something;
		set_cr3(pcb[i].Dir);
		pcb[i].tf_p->ebx = something;
		pcb[i].state = AVAIL;
		EnQue(&avail_que, i);
		for (i = 0; i < PAGE_MAX ; i++) if(page[i].pid == run_pid) page[i].pid = NONE;
	}
	set_cr3(KDir);
	
}


void SysSleep(void) {
	
	pcb[run_pid].wake_time = (sys_time_count + ((pcb[run_pid].tf_p->ebx) * 10));
	pcb[run_pid].state = SLEEP;
	run_pid = NONE;
	
}


void SysWrite(void) {
	
	unsigned short *old;	// jaja
	char *str= (char *)pcb[run_pid].tf_p->ebx;
    while( *str != '\0' ) {
		if(*str == '\r') {
			sys_cursor = ((((sys_cursor-VIDEO_START)/80)+1)*80)+VIDEO_START;
		} else {
			*sys_cursor = (*str)+VGA_MASK_VAL;
			sys_cursor++;
		}
		if(sys_cursor >= VIDEO_END) {
			old = VIDEO_START;
			sys_cursor = VIDEO_START;
			while(old != VIDEO_END) { *old++ = ' ' + VGA_MASK_VAL; }
		}
		str++;
	}
}


void SysLockMutex(void) {
	
	int mutex_id = pcb[run_pid].tf_p->ebx;
	
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


void SysFork(void) {
	
	int pidF, distance, *bpEbp, pgI;
	
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
		
		for (pgI = 0; pgI<PAGE_MAX; pgI++) { if(page[pgI].pid == NONE) break; }
		page[pgI].pid = pidF;
		pcb[pidF].Dir = KDir;
	}	
	
}

void SysSignal(void){ pcb[run_pid].signal_handler[pcb[run_pid].tf_p->ebx] = (func_p_t)pcb[run_pid].tf_p->edx; }

void SysSetCursor(void) { sys_cursor = VIDEO_START + ((pcb[run_pid].tf_p->ebx) * 80) + (pcb[run_pid].tf_p->edx); }
