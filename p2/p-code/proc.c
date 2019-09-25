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

	// declare two integers: my_pid & os_time
	int my_pid;
	int os_time;

	// declare two 20-char arrays: pid_str & time_str
	char pid_str[CHR_ARY];
	char time_str[CHR_ARY];


	// // call sys_get_pid() to get my_pid
	my_pid = sys_get_pid();
	
	// // call Number2Str() to convert it to pid_str
	// Number2Str(my_pid, pid_str);

	//forever loop 
	while(1){
		
		//call sys_write() to show "my PID is "
		sys_write("my PID is ");
		
		// //call sys_write() to show my pid_str
		// sys_write(pid_str);
		
		//call sys_write to show "... "
		sys_write("... ");
		
		//call sys_sleep() to sleep for 1 second
		sys_sleep(1);
		
		// // call sys_get_time() to get current os_time
		os_time = sys_get_time();
		//cons_printf("%d time\n", os_time);
		
		// //call Number2Str() to convert it to time_str
		// Number2Str(os_time, time_str);
		
		// // call sys_write() to show "sys time is "
		sys_write("sys time is ");
		
		// //call sys_write() to show time_str
		// sys_write(time_str);
		
		// // call sys_write to show "... "
		sys_write("... ");
		
		// //call sys_sleep() to sleep for 1 second
		sys_sleep(1);
		
	}
	
}
