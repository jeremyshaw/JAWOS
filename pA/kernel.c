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

tty_t tty;

unsigned int sys_time_count, KDir, sys_rand_count;
struct i386_gate *idt;

unsigned short *sys_cursor;

mutex_t video_mutex;

page_t page[PAGE_MAX];

void BootStrap(void) {
	
	int i;

	sys_time_count = 0;
	Bzero((char *) &avail_que, sizeof(que_t));
	Bzero((char *) &ready_que, sizeof(que_t));
	for(i = 0; i < QUE_MAX; i++) EnQue(&avail_que, i);
	sys_rand_count = 0;
	Bzero((char *) &video_mutex, sizeof(mutex_t));
	sys_cursor = VIDEO_START;
	
	Bzero((char *) &kb, sizeof(kb_t));
	
	idt = get_idt_base();
	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	fill_gate(&idt[SYSCALL_EVENT], (int)SyscallEntry, get_cs(), ACC_INTR_GATE, 0);
	fill_gate(&idt[TTY_EVENT], (int)TTYEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_MASK_REG, PIC_MASK_VAL);

	KDir = get_cr3();
	
	for(i = 0; i < PAGE_MAX; i++) {
		page[i].pid = NONE;
		page[i].u.addr = (unsigned int)(DRAM_START + (i * PAGE_SIZE));
	}
   
}


void TTYinit(void) {	// given code for phase 9

	int i, j;
	
	Bzero((char *)&tty, sizeof(tty_t));
	tty.port = TTY0;
	
	outportb(tty.port+CFCR, CFCR_DLAB);				// CFCR_DLAB is 0x80
	outportb(tty.port+BAUDLO, LOBYTE(115200/9600));	// period of each of 9600 bauds
	outportb(tty.port+BAUDHI, HIBYTE(115200/9600));
	outportb(tty.port+CFCR, CFCR_PEVEN|CFCR_PENAB|CFCR_7BITS);

	outportb(tty.port+IER, 0);
	outportb(tty.port+MCR, MCR_DTR|MCR_RTS|MCR_IENABLE);

	for(i=0; i<166667; i++)asm("inb $0x80");		// wait .1 sec
	outportb(tty.port+IER, IER_ERXRDY|IER_ETXRDY);	// enable TX & RX intr
	for(i=0; i<166667; i++)asm("inb $0x80");		// wait .1 sec

	for(j=0; j<3; j++) {							// clear 3 lines
		outportb(tty.port, 'V');
		for(i=0; i<83333; i++)asm("inb $0x80");		// wait .5 sec should do - this is half of the ".1" sec up above...
		outportb(tty.port, '\n');
		for(i=0; i<83333; i++)asm("inb $0x80");
		outportb(tty.port, '\r');
		for(i=0; i<83333; i++)asm("inb $0x80");
	}
	inportb(tty.port);								// get 1st key PROCOMM logo
	for(i=0; i<83333; i++)asm("inb $0x80");			// wait .5 sec
	
}


int main(void) {
	
	BootStrap();
	TTYinit();
	SpawnSR(&Idle);
	SpawnSR(&Login);
	run_pid = IDLE;
	set_cr3(pcb[run_pid].Dir);
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
		case TTY_EVENT:
			TTYSR();
			break;
		default:
			cons_printf("Kernel Panic: no such event!\n");
			breakpoint();
	}

	KBSR();
	Scheduler();
	set_cr3(pcb[run_pid].Dir);
	Loader(pcb[run_pid].tf_p);
	
}

