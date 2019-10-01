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
	while(1) { (sys_time_count % 100 < 50) ? (*start_pos = '*' + VGA_MASK_VAL) : (*start_pos = ' ' + VGA_MASK_VAL); }
}	// now it's a "reasonably" accurate flash with 1 second period, 50% duty cycle.


void Init(void) {  // Init, PID 1, asks/tests various OS services
	
	int my_pid, os_time, counter, forked_pid;
	char pid_str[CHR_ARY], time_str[CHR_ARY];

	counter = 2;
	while(counter--){
		forked_pid = sys_fork();
		if(forked_pid == NONE) sys_write("sys_fork() failed!\n");
	}
	
	my_pid = sys_get_pid();               // what's my PID
	Number2Str(my_pid, pid_str);          // convert # to str

	while(1) {
		
		sys_sleep(1);
		sys_set_cursor(my_pid, 0);	// set position to row my_pid, column 0
		sys_write("my PID is ");	// sys_write to show pid
		sys_write(pid_str);
		sys_write("... ");
		os_time = sys_get_time();	// get time, show it, and sleep 1 sec
		Number2Str(os_time, time_str);
		
		sys_sleep(1);
		sys_set_cursor(my_pid, 0);	// set cursor back again
		sys_write("sys time is ");	// sys_write to show the time
		sys_write(time_str);
		sys_write("... ");
		
	}
}

