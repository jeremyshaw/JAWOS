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
int run_pid;
//extern int run_pid;?

//declare 2 queues: avail_que and ready_que;  // avail PID and those created/ready to run
que_t avail_que;
que_t ready_que;

//declare an array of PCB type: pcb[PROC_MAX];                // Process Control Blocks
pcb_t pcb[PROC_MAX];

//declare an unsigned integer: sys_time_count
unsigned int sys_time_count;
struct i386_gate *idt;         // interrupt descriptor table

char ch;//for kb capture breakpoint

int i;//for loops

void BootStrap(void){         // set up kernel!
    //set sys time count to zero
	sys_time_count = 0;
	
	// call tool Bzero(char *)&avail_que, sizeof(que_t)) to clear avail queue
	Bzero((char *) &avail_que, sizeof(que_t));

	// call tool Bzero() to clear ready queue
	Bzero((char *) &ready_que, sizeof(que_t));

	//enqueue all the available PID numbers to avail queue
	for(i = 0; i < QUE_MAX; i++){
		EnQue(&avail_que, i);
	}
	// cons_printf("avail_que.tail = %d\n", avail_que.tail);
	// for(i = 0; i < QUE_MAX; i++){
		// cons_printf("avail_que.que[%d] %d\n", i, avail_que.que[i]);
	// }
	// breakpoint();

	//get IDT location//lot of the following and this line done in prep4
	//addr of TimerEntry is placed into proper IDT entry //32?
	//send PIC control register the mask value for timer handling
	
	idt = get_idt_base();
	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
		
	//from prep4
	// idt = get_idt_base();
	// fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	// outportb(PIC_MASK_REG, PIC_MASK_VAL);
}

int main(void) {               // OS starts
	//do the boot strap things 1st
	BootStrap();

	SpawnSR(IDLE);              // create Idle thread

	//set run_pid to IDLE (defined constant)
	run_pid = IDLE;
	
	cons_printf("run_pid is now IDLE\n");
	breakpoint();
	//call Loader() to load the trapframe of Idle
	Loader(pcb[IDLE].tf_p);
	cons_printf("rLoaded trapframe of Idle\n");
	breakpoint();

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
	
	while(1){
		if(cons_kbhit()){
			ch = cons_getchar();
			cons_printf(" pressed");
			if(ch=='b')breakpoint();
		}
		
	}

	//call Scheduler() to change run_pid if needed
	Scheduler();
   
	//call Loader() to load the trapframe of the selected process
	Loader(tf_p);
	//Loader(pcb[run_pid], tf_p);
}

