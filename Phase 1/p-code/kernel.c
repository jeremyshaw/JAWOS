// kernel.c, 159, phase 1
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


//declare an integer: run_pid;                        // current running PID; if -1, none selected
extern int run_pid;
//extern int run_pid;?

//declare 2 queues: avail_que and ready_que;  // avail PID and those created/ready to run
extern que_t avail_que;
extern que_t ready_que;

//declare an array of PCB type: pcb[PROC_MAX];                // Process Control Blocks
extern pcb_t pcb[PROC_MAX];

//declare an unsigned integer: sys_time_count
extern unsigned int sys_time_count;
extern struct i386_gate *idt;         // interrupt descriptor table

void BootStrap(void) {         // set up kernel!
    //set sys time count to zero
	sys_time_count = 0;
	
	// call tool Bzero(char *)&avail_que, sizeof(que_t)) to clear avail queue
	Bzero((char *) &avail_que, sizeof(que_t));

	// call tool Bzero() to clear ready queue
	Bzero((char *) &ready_que, sizeof(que_t));

	enqueue all the available PID numbers to avail queue


	get IDT location//lot of the following and this line done in prep4
	addr of TimerEntry is placed into proper IDT entry //32?
	send PIC control register the mask value for timer handling
}

int main(void) {               // OS starts
	do the boot strap things 1st

	SpawnSR(Idle);              // create Idle thread

	//set run_pid to IDLE (defined constant)
	run_pid = IDLE;

	//call Loader() to load the trapframe of Idle
	Loader(Idle);

	return 0; // never would actually reach here
}

void Scheduler(void) {              // choose a run_pid to run
	if(run_pid > IDLE) return;       // a user PID is already picked

	if(QueEmpty(&ready_que)) {
	  run_pid = IDLE;               // use the Idle thread
	} else {
	  pcb[IDLE].state = READY;
	  run_pid = DeQue(&ready_que);  // pick a different proc
	}

	pcb[run_pid].time_count = 0;     // reset runtime count
	pcb[run_pid].state = RUN;
}

void Kernel(tf_t *tf_p) {       // kernel runs
	//copy tf_p to the trapframe ptr (in PCB) of the process in run
	pcb[run_pid].tf_p = tf_p;

    //call the timer service routine
    TimerSR(); //incomplete?

    //if 'b' key on target PC is pressed, goto the GDB prompt -->breakpoint()?
   	asm("sti"); //set to be ready for ints, from Phase0
	char ch;
	while(1){
		if(cons_kbhit()){
			ch = cons_getchar();
			cons_printf(" pressed");
			if(ch=='b')breakpount();
		}
		
	}

	call Scheduler() to change run_pid if needed
	//if(??needed)Scheduler();
   
	//call Loader() to load the trapframe of the selected process
	Loader(*tf_p);
	//Loader(pcb[run_pid], tf_p);
}

