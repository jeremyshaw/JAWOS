// const-type.h, 159, needed constants & types
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _CONST_TYPE_           // to prevent name mangling recursion //"if not define, define"
#define _CONST_TYPE_           // to prevent name redefinition //(continued...) uses the end if at the very end. Sometime about recursive stuff.

#define TIMER_EVENT 32         // timer interrupt signal code
#define PIC_MASK_REG 0x21      // I/O loc # of PIC mask
#define PIC_MASK_VAL ~0x01     // mask code for PIC
#define PIC_CONT_REG 0x20      // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60  // control code sent to PIC
#define VGA_MASK_VAL 0x0f00    // bold face, white on black

#define TIME_MAX 310            // max timer count, then rotate process
#define PROC_MAX 20             // max number of processes
#define STACK_MAX 4096          // process stack in bytes
#define QUE_MAX 20              // capacity of a process queue

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

#define SYS_WRITE_DECAY 150

#define VIDEO_MUTEX 0
#define LOCKED 1
#define UNLOCKED 0	// this works because Bzero in Bootstrap

#define VIDEO_START (unsigned short *) 0xb8000 //uns short is 2 byte pointer in this case
#define VIDEO_END (unsigned short *) 0xb8000 + 25 * 80


typedef void(*func_p_t)(void);

typedef enum {AVAIL, READY, RUN, SLEEP, SUSPEND} state_t;
#define AVAIL 0
#define READY 1
#define RUN 2
#define SLEEP 3
#define SUSPEND 4

typedef struct {
	int tail;
	int que[QUE_MAX];
} que_t;

// I liked this better :D
// typedef enum {LOCKED, UNLOCKED} lock_t;	// make sure this name is okay
// #define LOCKED 1
// #define UNLOCKED 0

typedef struct {
	int lock;
	que_t suspend_que;
} mutex_t;	// make sure this name is okay

typedef struct {
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax, event, eip, cs, efl;
} tf_t;                      

typedef struct {
	state_t state;
	tf_t *tf_p;
	unsigned int time_count, total_time, wake_time, ppid; //ppid
} pcb_t;

#endif                          // to prevent name mangling
