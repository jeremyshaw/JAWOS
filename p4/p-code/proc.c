// proc.c, 159 JAWOS
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "proc.h"


#define CHR_ARY 20	// max charcter array length


void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner

	unsigned short *start_pos = (unsigned short *)0xb8000;
	int flag = 0; //flat = 1 = display

	while(1) { 
		sys_rand_count++;	// p4
		if(sys_time_count % 100 == 0) {
			if(flag == 1) {
				*start_pos = '*' + VGA_MASK_VAL;
				flag = 0;
			}
			else {
				*start_pos = ' ' + VGA_MASK_VAL;
				flag = 1;
			}
		}
	}
}



void Init(void) {    // illustrates a racing condition
	int col, my_pid, forked_pid, rand, i, j;
	char pid_str[20];

	forked_pid = sys_fork();
	if(forked_pid == NONE)sys_write("sys_fork() failed!\n");

	forked_pid = sys_fork();
	if(forked_pid == NONE)sys_write("sys_fork() failed!\n");

	my_pid = sys_get_pid();              // what's my PID
	Number2Str(my_pid, pid_str);         // convert # to str

	while(1){
		col = 0;	// start column with 0
		sys_set_cursor(0, col);
		for (i = 0; i < 70; i++) {	// add a subloop (to loop until column reaches 70):
			sys_lock_mutex(VIDEO_MUTEX);	// lock video mutex
			sys_set_cursor(i, col);	// set video cursor
			sys_write(pid_str);	// write my PID
			sys_unlock_mutex(VIDEO_MUTEX);	// unlock video mutex
			rand = sys_get_rand();	// get a number ranging from 1 to 4 inclusive randomly
			sys_sleep(ranOut);	// call sleep with that number as sleep period
			col++;	// increment column by 1
		}
		
		// erase my entire row (use mutex & loop, of course)
		sys_lock_mutex(VIDEO_MUTEX);
		for(j = 0; j < 70; j++) { sys_write(' '); }
		sys_unlock_mutex(VIDEO_MUTEX);
		
		sys_sleep(30);	// sleep for 30 (3 seconds)
	}
}


