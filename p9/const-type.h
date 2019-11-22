// const-type.h, 159, needed constants & types
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _CONST_TYPE_           // to prevent name mangling recursion //"if not define, define"
#define _CONST_TYPE_           // to prevent name redefinition //(continued...) uses the end if at the very end. Sometime about recursive stuff.

#define TIMER_EVENT 32         // timer interrupt signal code
#define PIC_MASK_REG 0x21      // I/O loc # of PIC mask
// #define PIC_MASK_VAL ~0x01     // mask code for PIC
#define PIC_MASK_VAL ~0x09		// ~0...01001
#define PIC_CONT_REG 0x20      // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60  // control code sent to PIC
#define VGA_MASK_VAL 0x0f00    // bold face, white on black
#define TTY_SERVED_VAL 0x63		// also for COM4; 0x64 for COM3

#define CONSOLE 100
#define TTY 200
#define TTY_EVENT 35	// TTY0/2; use 36 for TTY1
#define TTY0 0x2F8	// TTY1 0x3e8, TTY2 0x2e8

#define TIME_MAX 310            // max timer count, then rotate process
#define PROC_MAX 20             // max number of processes
#define STACK_MAX 4096          // process stack in bytes
#define QUE_MAX 20              // capacity of a process queue
#define STR_MAX 20				// "official" max char arry len
#define PAGE_MAX 100

#define PAGE_SIZE 4096
#define G1 0x40000000
#define G2 0x80000000
#define PRESENT 0x01
#define RW 0x02
#define RO 0x00

#define NONE -1                 // to indicate none
#define IDLE 0                  // Idle thread PID 0

#define DRAM_START 0xe00000     // 14 MB

#define SYSCALL_EVENT 128       // syscall event identifier code, phase2
#define SYS_GET_PID 129         // different types of syscalls
#define SYS_GET_TIME 130
#define SYS_SLEEP 131
#define SYS_WRITE 132
#define SYS_FORK 133
#define SYS_SET_CURSOR 134
#define SYS_GET_RAND 135
#define SYS_LOCK_MUTEX 136
#define SYS_UNLOCK_MUTEX 137
#define SYS_EXIT 138
#define SYS_WAIT 139
#define SYS_SIGNAL 140
#define SYS_KILL 141
#define SYS_READ 142
#define SYS_VFORK 143

#define VIDEO_MUTEX 0
#define LOCKED 1
#define UNLOCKED 0

#define VIDEO_START (unsigned short *) 0xb8000 //uns short is 2 byte pointer in this case
#define VIDEO_END (unsigned short *) 0xb8000 + 25 * 80


typedef void(*func_p_t)(void);

typedef enum {AVAIL, READY, RUN, SLEEP, SUSPEND, WAIT, ZOMBIE, IO_WAIT} state_t;
#define AVAIL 0
#define READY 1
#define RUN 2
#define SLEEP 3
#define SUSPEND 4
#define WAIT 5
#define ZOMBIE 6
#define IO_WAIT 7

// typedef enum {SIGCHLD, SIGCONT} signal_t;
#define SIGCHLD 17
#define SIGCONT 18


typedef struct {
	int tail;
	int que[QUE_MAX];
} que_t;

typedef struct {
	char *str;	// addr of string to print
	que_t wait_que;	//requesting process
	int port;	// set to TTY0/1/2/
} tty_t;

typedef struct {
	int pid;
	union {
		unsigned addr;
		char *content;
		unsigned *entry;
	} u;
} page_t;

typedef struct {
	que_t buffer;
	que_t wait_que;
} kb_t;

typedef struct {
	int lock;
	que_t suspend_que;
} mutex_t;

typedef struct {
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax, event, eip, cs, efl;
} tf_t;                      

typedef struct {
	state_t state;
	tf_t *tf_p;
	unsigned int time_count, total_time, wake_time, ppid, Dir;
	int STDOUT;
	func_p_t signal_handler[32];
} pcb_t;

#endif                          // to prevent name mangling
