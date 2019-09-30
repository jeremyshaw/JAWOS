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

// declare kernel data

// aren't all of these in ext-data.h?
int run_pid; 	// current running PID; if -1, none selected

que_t avail_que; //avail pid
que_t ready_que; //created/ready to run pid

pcb_t pcb[PROC_MAX];

unsigned int sys_time_count;
struct i386_gate *idt;

unsigned short *sys_cursor;         // phase2

unsigned sys_rand_count;
mutex_t video_mutex;

char ch;//for kb capture breakpoint
int i;


void BootStrap(void) {

	sys_time_count = 0;

	Bzero((char *) &avail_que, sizeof(que_t));
	Bzero((char *) &ready_que, sizeof(que_t));

	
	for(i = 0; i < QUE_MAX; i++) EnQue(&avail_que, i);	// enqueue all available PID num to avail_que
	
	// add code to initialize sys_rand_count and video_mutex

	sys_cursor = VIDEO_START;  // have it set to VIDEO_START in BootStrap()

	idt = get_idt_base();
	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	fill_gate(&idt[SYSCALL_EVENT], (int)SyscallEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_MASK_REG, PIC_MASK_VAL);
   
}


int main(void) {	// kernel boots
	
	BootStrap();

	SpawnSR(&Idle);
	SpawnSR(&Init);

	run_pid = IDLE;	// set run_pid to IDLE (defined constant)
	
	Loader(pcb[run_pid].tf_p);

	return 0; // never would actually reach here
	
}


void Scheduler(void) {              // choose a run_pid to run

	if( run_pid > IDLE ) return;       // a user PID is already picked

	if( QueEmpty(&ready_que) ) run_pid = IDLE;               // use the Idle thread
	else {
		pcb[IDLE].state = READY;
		run_pid = DeQue(&ready_que);  // pick a different proc
	}

	pcb[run_pid].time_count = 0;     // reset runtime count
	pcb[run_pid].state = RUN;
	
}


void Kernel(tf_t *tf_p) {       // kernel runs
	
	pcb[run_pid].tf_p = tf_p;

	switch(tf_p->event) {
		case TIMER_EVENT:
			TimerSR();         // handle tiemr event
			break;
		case SYSCALL_EVENT:
			SyscallSR();       // all syscalls go here 1st
			break;
		default:
			cons_printf("Kernel Panic: no such event!\n");
			breakpoint();
	}

	if(cons_kbhit()) {           // if keyboard pressed
		ch = cons_getchar();
		cons_printf("%c pressed", ch);
		if(ch=='b') breakpoint();	
	}
	
	Scheduler();
	Loader(pcb[run_pid].tf_p);
	
}

