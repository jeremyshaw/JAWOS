// ksr.c, 159 Kernal Service Routines

//need to include spede.h, const-type.h, ext-data.h, tools.h
#include"spede.h";
#include"const-type.h";
#include"ext-data.h";
#include"tools.h";

// to create a process: alloc PID, PCB, and process stack
// build trapframe, initialize PCB, record PID to ready_que
void SpawnSR(func_p_t p) {     // arg: where process code starts
   int pid;

   use a tool function to check if available queue is empty:
      a. cons_printf("Panic: out of PID!\n");
      b. and go into GDB

   get 'pid' initialized by dequeuing the available queue
   use a tool function to clear the content of PCB of process 'pid'
   set the state of the process 'pid' to READY

   if 'pid' is not IDLE, use a tool function to enqueue it to the ready queue 

   use a tool function to copy from 'p' to DRAM_START, for STACK_MAX bytes

   create trapframe for process 'pid:'
   1st position trapframe pointer in its PCB to the end of the stack
   set efl in trapframe to EF_DEFAULT_VALUE|EF_INTR  // handle intr
   set cs in trapframe to return of calling get_cs() // duplicate from CPU
   set eip in trapframe to DRAM_START                // where code copied
}


// count run time and switch if hitting time limit
void TimerSR(void) {
   1st notify PIC control register that timer event is now served

   increment system time count by 1
   increment the time count of the process currently running by 1
   increment the life span count of the process currently running by 1

   if the time count of the process is reaching maximum allowed runtime
      move the process back to the ready queue
      alter its state to indicate it is not running but ...
      reset the PID of the process in run to NONE
   }
}

