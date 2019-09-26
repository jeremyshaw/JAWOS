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


void Init(void) {  // Init, PID 1, asks/tests various OS services
	
	int my_pid, os_time, counter, forked_pid;
	char pid_str[CHR_ARY], time_str[CHR_ARY];

	counter = 2;
	
	forked_pid = sys_fork();
	if(forked_pid == NONE) sys_write("sys_fork() failed!\n");
	//sys_write("sys_fork() called \n");
	
	// forked_pid = sys_fork();
	// if(forked_pid == NONE) sys_write("sys_fork() failed!\n");
	//sys_write("sys_fork() called \n");
	

	my_pid = sys_get_pid();               // what's my PID
	Number2Str(my_pid, pid_str);          // convert # to str

	while(1) {
		
		sys_sleep(1);
		//set cursor position to my row (equal to my PID), column 0,
		sys_set_cursor(my_pid, 0);
		
		// call sys_write a few times to show my PID as before,
		sys_write("my PID is ");
		sys_write(pid_str);
		sys_write("... ");

		// get time, and convert it, sleep for a second,
		os_time = sys_get_time();
		Number2Str(os_time, time_str);
		sys_sleep(1);
		
		//set cursor position back again, (my_pid or row 0? I assume my_pid for row)
		sys_set_cursor(my_pid, 0);
		
		//call sys_write a few times to show sys time as before.
		sys_write("sys time is ");
		sys_write(time_str);
		sys_write("... ");
	}
}

