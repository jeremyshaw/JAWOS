// proc.c, 159
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()

#define CHR_ARY 20

void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner
...
}

void Init(void) {  // Init, PID 1, asks/tests various OS services
	// declare two integers: my_pid & os_time
	int my_pid;
	int os_time;

	// declare two 20-char arrays: pid_str & time_str
	char pid_str[CHR_ARY];
	char time_str[CHR_ARY];


	// ?? check back on this ?? call sys_get_pid() to get my_pid
	my_pid = sys_get_pid();
	
	// ?? check back on this ?? call Number2Str() to convert it to pid_str
	Number2Str(my_pid, &pid_str);

	forever loop 
	while(1){
		call sys_write() to show "my PID is "
		call sys_write() to show my pid_str
		call sys_write to show "... "
		call sys_sleep() to sleep for 1 second
		call sys_get_time() to get current os_time
		call Number2Str() to convert it to time_str
		call sys_write() to show "sys time is "
		call sys_write() to show time_str
		call sys_write to show "... "
		call sys_sleep() to sleep for 1 second
	}
}
