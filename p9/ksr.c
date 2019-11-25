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
	pcb[pid].STDOUT = ((pid == 0) ? (CONSOLE) : (TTY));	// set STDOUT; CONSOLE for pid==0
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



void TTYSR(void){

	int pid, i;
	char ttych;
	outportb(PIC_CONT_REG, TTY_SERVED_VAL);
	
	if(QueEmpty(&tty.wait_que)) return;
	pid = tty.wait_que.que[0];
	
	set_cr3(pcb[pid].Dir);
	
	ttych = *(tty.str);
	if( ttych != 0 ) {
		if(ttych == '\r') outportb(tty.port, '\n');
		for(i=0; i<3333; i++)asm("inb $0x80");	// 83333
		outportb(tty.port, ttych);
		tty.str++;
	} else {
		pid = DeQue(&tty.wait_que);
		pcb[pid].state = READY;
		EnQue(&ready_que, pid);
	}
	
}


void KBSR(void) {
	
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
	
	unsigned int Dir, *KKDir;
	int IT, DT, IP, DP, pidF, i, pageNum, pageIndex[5];

	pageNum = 0;
	if(QueEmpty(&avail_que)) {
		cons_printf("Not enough PID\n");
		breakpoint();		
	} else {
		pidF = DeQue(&avail_que);
		EnQue(&ready_que, pidF);
		
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

		for(i = 0; i < 5; i ++) {
			page[pageIndex[i]].pid = pidF;
			Bzero(page[pageIndex[i]].u.content, PAGE_SIZE);
		}
		Dir = pageIndex[0];
		IT = pageIndex[1];
		DT = pageIndex[2];
		IP = pageIndex[3];
		DP = pageIndex[4];
		
		KKDir = (int *)KDir;
		for (i = 0; i < 16; i++) page[Dir].u.entry[i] = KKDir[i];
		page[Dir].u.entry[256] = page[IT].u.addr|PRESENT|RW;
		page[Dir].u.entry[511] = page[DT].u.addr|PRESENT|RW;
		
		page[IT].u.entry[0] = page[IP].u.addr|PRESENT|RO;
		page[DT].u.entry[1023] = page[DP].u.addr|PRESENT|RW;
		
		// build IP - copy instructions to IP (src addr is ebx of TF)
		MemCpy((char *)page[IP].u.addr, (char *)(pcb[run_pid].tf_p->ebx), PAGE_SIZE);
		
		page[DP].u.entry[1021] = G1;	// 3rd to last in u.entry[] is eip = G1
		page[DP].u.entry[1022] = get_cs();	// 2nd to last in u.entry[] is cs = get_cs()
		page[DP].u.entry[1023] = EF_DEFAULT_VALUE|EF_INTR;	// the last in u.entry[] is efl, = EF_DEF... (like SpawnSR)
		
		pcb[pidF].Dir = page[Dir].u.addr;
		pcb[pidF].tf_p = (tf_t*)(G2 - sizeof(tf_t));	// tf_p in PCB of new process has VA = G2 - size_of trapframe	
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
	
	int i, pid, signal_name;
	
	pid = pcb[run_pid].tf_p->edx;
	signal_name = pcb[run_pid].tf_p->ebx;
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
	
	int ppid, i, something;
	
	ppid = pcb[run_pid].ppid;
	something = pcb[run_pid].tf_p->ebx;
	set_cr3(pcb[ppid].Dir);
	
	if(pcb[ppid].state == WAIT) {
		pcb[ppid].state = READY;
		EnQue(&ready_que, ppid);
		pcb[ppid].tf_p->edx = run_pid;
		*((int *)pcb[ppid].tf_p->ebx) = something;
		set_cr3(pcb[run_pid].Dir);
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
	
	int i, something, j;	// we are using i as the "found pid" for this function	
	for(i = 0; i < PROC_MAX; i++) { if(pcb[i].state == ZOMBIE && pcb[i].ppid == run_pid) break; }
	
	if(i == PROC_MAX){	// nothing is waiting, nothing to run
		pcb[run_pid].state = WAIT;
		run_pid = NONE;
	} else {
		set_cr3(pcb[i].Dir);
		something = pcb[i].tf_p->ebx;
		pcb[i].state = AVAIL;
		set_cr3(pcb[run_pid].Dir);
		pcb[run_pid].tf_p->edx = i;
		*((int *)pcb[run_pid].tf_p->ebx) = something;
		EnQue(&avail_que, i);
		for (j = 0; j < PAGE_MAX ; j++) if(page[j].pid == i) page[j].pid = NONE;	// reclaim pages; updated from phase8 grading comment
	}
	set_cr3(KDir);
	
}


void SysSleep(void) {
	
	pcb[run_pid].wake_time = (sys_time_count + ((pcb[run_pid].tf_p->ebx) * 10));
	pcb[run_pid].state = SLEEP;
	run_pid = NONE;
	
}


void SysWrite(void) {
	
	unsigned short *old;
	char *str= (char *)pcb[run_pid].tf_p->ebx;
	if (pcb[run_pid].STDOUT == CONSOLE) {
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
	} else if(pcb[run_pid].STDOUT == TTY) {
         tty.str = str;	
         EnQue(&tty.wait_que, run_pid);
         pcb[run_pid].state = IO_WAIT;
         run_pid = NONE;
		 TTYSR();
	} else {
		cons_printf("no such device!");
		breakpoint();
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
