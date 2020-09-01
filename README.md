# JAWOS (Jeremy, Alex, & Will Operating System)
CSU Sacramento CSC159/CpE159 Operating System Pragmatics
Fall 2019

## What is JAWOS?
JAWOS (Jeremy, Alex, & Will Operating System) is an x86, ATX compatible OS, specifically targeting an ~386 era PC with an attached serial teletype console, VGA-compatible video system, and ANSI keyboard. It's largely written in C, with our own structures where neccessary, and x86 assembly.

## JAWOS Overview
(WIP, block diagram)

## What are Phases?
Steps from which the OS is constructed, piece by piece.

## Who?
* Jeremy Shaw [jeremyshaw](https://github.com/jeremyshaw), Computer Engineering
* Alex Leones [aleo101](https://github.com/aleo101), Computer Engineering
* Will Guo  [williamguo908](https://github.com/williamguo908), Computer Science

## Professor
Weide Chang [CSUS](http://athena.ecs.csus.edu/~changw/)

## Phases 0 through A
Attempted to preserve the psuedocode ("p-code") when possible and the READEME files (which assigned the work for each Phase and expected outcomes) as well.

### Phase 0
Prep, individual. The one here is Jeremy's.

### Phase 1
Setting up the structure of the OS that we will create.
Team is formed here, with Jeremy Shaw, Alex Leones, and William Guo
```C
fill_gate();	\\ setup an entry in the Interrupt Descriptor Table (IDT), a hub for interrupt vectors.
get_idt_base();	\\ find where the IDT itself is located in address space (AFAIK, since this is )
```

### Phase 2
Create a second process that shares CPU time
```C
Idle();	\\ system Idle process - PID 0.
Init();	\\ new process
```

### Phase 3
Implemented process forking. This required analysis of the C program memory structure (TDHS - Text Data Heap Stack) and the various registers that are stored in the process control block (specifically, base pointers and return addresses for parent and children).
```C
int sys_fork();	\\ returns child pid to parent, 0 to child
```

### Phase 4
mutex added; right now, for video output (text) only. Specifically, this means we only have one mutex, and we use it to control the video output text renderer.
```C
sys_unlock_mutex(mutex_ID);	\\ Unlock mutex
sys_lock_mutex(mutex_ID);	\\ Lock mutex
```
Jeremy also added a matrix text crawl. It's very basic, however. Lots of fun learning about the VGA control registers and the different VGA code pages (#437 in particular). This demo is in the attached "fun" subdirectory of this phase and is not a faithfully accurate rendition of the text crawl.

### Phase 5
Exit (for children)
Wait (for parent)
```C
sys_exit(INT - pass to parent);	\\ for child to pass info to parent
sys_wait();	\\ Used by parent to wait for a child to exit
```

### Phase 6
System signalling. This came from a question on the midterm, specifically asking for us to postulate about a way somewhat synchronize processes.
```C
sys_signal();	\\ Used to setup signal handler
sys_kill();	\\ Use to send signal, in p6, we use this to send a SIGCONT to skip a sys_sleep(large value INT). This sys_sleep was being used as a weak method for freezing/pausing a program/thread until the SIGCONT signal was received. 
```

### Phase 7
Kernel input "driver" (for reading text from KB)
```C
sys_read();	\\ Used to capture KB inputs; the first two input strings are slightly offset. The first one should always miss all inputs.
StrCmp();	\\ Wrote a stringcompare from memory. With how bad the inputs were being garbled in this phase, I wrote this with a failsafe mentality
```

### Phase 8
Virtual Memory
```C
sys_vfork();	\\ more like a "vSpawnSR" with some basis on a "simple" fork, since we use this to spawn a new process
```

### Phase 9
TTY (simple, unidirectional output only)
```C
TTYSR();	\\ This is called by the PIC automatically, via a set HW timer. Handles outputting over serial
```

### Phase A
Bidirectional TTY
```C
TTYSR();	\\ now serves as dispatcher for TTY in/out
TTYdspSR();	\\ displays outputs to the Serial Terminal
TTYkbSR();	\\ captures input from the Serial Terminal's KB
```


