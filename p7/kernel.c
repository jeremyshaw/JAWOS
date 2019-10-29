// kernel.c, 159, phase 2
//
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "entry.h"    // entries to kernel (TimerEntry, etc.)
#include "tools.h"    // small handy functions
#include "ksr.h"      // kernel service routines
#include "proc.h"     // all user process code here
#include "ext-data.h"
#include "syscall.h"

int run_pid; 	// current running PID; if -1, none selected

que_t avail_que; //avail pid
que_t ready_que; //created/ready to run pid

kb_t kb;

pcb_t pcb[PROC_MAX];

unsigned int sys_time_count;
struct i386_gate *idt;

unsigned short *sys_cursor;
unsigned sys_rand_count;
mutex_t video_mutex;

char ch;
int i;

void BootStrap(void) {

	sys_time_count = 0;
	Bzero((char *) &avail_que, sizeof(que_t));
	Bzero((char *) &ready_que, sizeof(que_t));
	Bzero((char *) &kb, sizeof(que_t));
	for(i = 0; i < QUE_MAX; i++) EnQue(&avail_que, i);
	sys_rand_count = 0;
	Bzero((char *) &video_mutex, sizeof(mutex_t));
	sys_cursor = VIDEO_START;

	idt = get_idt_base();
	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	fill_gate(&idt[SYSCALL_EVENT], (int)SyscallEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_MASK_REG, PIC_MASK_VAL);
   
}


int main(void) {
	
	BootStrap();
	SpawnSR(&Idle);
	SpawnSR(&Login);
	run_pid = IDLE;
	Loader(pcb[run_pid].tf_p);
	
	return 0; // never would actually reach here
	
}


void Scheduler(void) { 

	if( run_pid > IDLE ) return;

	if( QueEmpty(&ready_que) ) run_pid = IDLE;
	else {
		pcb[IDLE].state = READY;
		run_pid = DeQue(&ready_que);
	}

	pcb[run_pid].time_count = 0;
	pcb[run_pid].state = RUN;
	
}


void Kernel(tf_t *tf_p) {
	
	pcb[run_pid].tf_p = tf_p;

	switch(tf_p->event) {
		case TIMER_EVENT:
			TimerSR(); 
			break;
		case SYSCALL_EVENT:
			SyscallSR();
			break;
		default:
			cons_printf("Kernel Panic: no such event!\n");
			breakpoint();
	}

	// keep this util debug routing in new process and syscall works
	if(cons_kbhit()) { 
		ch = cons_getchar();
		cons_printf("%c pressed. ", ch);
		if(ch=='$') breakpoint();	
		// if(ch==' ') SpawnSR(&Init);
	}
	
	Scheduler();
	Loader(pcb[run_pid].tf_p);
	
}

