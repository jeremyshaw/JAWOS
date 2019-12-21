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


	my_pid = sys_get_pid();
	
	Number2Str(my_pid, pid_str);

	while(1){
		
		sys_write("my PID is ");
		
		sys_write(pid_str);
		
		sys_write("... ");
		
		sys_sleep(1);
		
		os_time = sys_get_time();
		
		Number2Str(os_time, time_str);
		
		sys_write("sys time is ");
		
		sys_write(time_str);
		
		sys_write("... ");
		
		sys_sleep(1);
		
	}
	
}
