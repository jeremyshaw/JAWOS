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
	while(1) { 
		if (sys_time_count % 100 < 50)
			*start_pos = '*' + VGA_MASK_VAL;
		else *start_pos = ' ' + VGA_MASK_VAL; 
		sys_rand_count++;	// part of the "random" counter	
	}
}	// now it's a "reasonably" accurate flash with 1 second period, 50% duty cycle.


void Init(void) {
	char pid_str[CHR_ARY], str[CHR_ARY];
	int my_pid, forked_pid, i, col, exit_pid, exit_code, sleep_period, total_sleep_period;

	for (i = 0; i < 5; i++) {
		forked_pid = sys_fork();	// call sys_fork() and get its function return as forked_pid
		if(forked_pid == 0) break;	// a child process now breaks the loop [where is the code for this?]
		if (forked_pid == NONE) {
			sys_write("sys_fork() failed!\n");
			sys_exit(NONE);	// call exit with code NONE
		}
	}
	
	my_pid = sys_get_pid();
	Number2Str(my_pid, pid_str);
	
	if(forked_pid != 0){	// for the one parent process, it does
		for (i = 0; i < 5; i++) {
			exit_pid = sys_wait(&exit_code);	// call sys_wait() with addr of exit_code as argument, 
			// sys_wait returns an integer, received as exit_pid
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, i*14); 
			sys_write("PID ");
			Number2Str(exit_pid, str);
			sys_write(str);
			sys_write(": ");
			Number2Str(exit_code, str);
			sys_write(str);
			sys_unlock_mutex(VIDEO_MUTEX);
		}
		sys_write("  Init exits.");
		sys_exit(0);	// call exit with code 0;
	}

	// child code below, similar to prev, race across screen
	total_sleep_period = 0;
	col = 0;

	while (col < 70) {	// revert this back to 70 before submitting
	
		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, col);
		sys_write(pid_str);
		sys_unlock_mutex(VIDEO_MUTEX);

		sleep_period = (sys_get_rand()/my_pid) % 4 + 1;	// get a random sleep_period = ... // get rand # 1~4
		sys_sleep(sleep_period);	// sleep with that random period
		total_sleep_period += sleep_period;	// add random period to total_sleep_period
		col++;
	
	}
	
	sys_exit(total_sleep_period);	// call exit sesrvice with total_sleep_period as exit code

}

