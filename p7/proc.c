// proc.c, 159 JAWOS
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "proc.h"


void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner
	unsigned short *start_pos = (unsigned short *)0xb8000;
	while(1) { 
		if (sys_time_count % 100 < 50)
			*start_pos = '*' + VGA_MASK_VAL;
		else *start_pos = ' ' + VGA_MASK_VAL; 
		sys_rand_count++;	// part of the "random" counter	
	}
}



int StrCmp (char *a, char *b){
	
	int diff;	// implementing a classical StrCmp from memory
	//	let's hope it works!	
	while(a != '\0' && b != '\0') {
		
		diff = (int)a-(int)b;
		
	}
	return diff;
}




void Login(void) {
	
	char login_str[STR_MAX], passwd_str[STR_MAX];
	int strcmp;
	
	while(1) {
		
		sys_write("login: ");
		sys_read(login_str);
		sys_write("password: ");
		sys_read(passwd_str);
		strcmp = StrCmp(login_str, passwd_str);
		if(strcmp) sys_write("prompt: login failed!\r");
		else break;
		
	}
	sys_write("prompt: login sucessful!\r");
	// sys_vfork(Shell); phase 8
}
















void MyChildExitHandler(void) {	// the handler when a child process exits:

	int cpid, ec;
	char cpidstr[STR_MAX], ecstr[STR_MAX];
	  
	cpid = sys_wait(&ec);	// call sys_wait() to get exiting child PID and exit code
	
	Number2Str(cpid, cpidstr);
	Number2Str(ec, ecstr);

	sys_lock_mutex(VIDEO_MUTEX);
	sys_set_cursor(cpid, 72);	// set the video cursor to row: exiting child pid, column: 72
	sys_write(cpidstr);
    sys_write(":");
	sys_write(ecstr);
    sys_unlock_mutex(VIDEO_MUTEX);
	
}


void Init(void) {
	char pid_str[STR_MAX];
	int my_pid, forked_pid, i, col, sleep_period, total_sleep_period;
	
	sys_signal(SIGCHLD, MyChildExitHandler);

	for (i = 0; i < 5; i++) {
		forked_pid = sys_fork();
		if(forked_pid == 0) break;	// child breaks loop
		if (forked_pid == NONE) {
			sys_write("sys_fork() failed!\n");
			sys_exit(NONE);
		}
	}
	
	my_pid = sys_get_pid();
	Number2Str(my_pid, pid_str);
	
	if(forked_pid != 0){	// for the one parent process
		sys_sleep(10);
		sys_kill(SIGCONT, 0);
		while(1) {
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, 0);
			sys_write(pid_str);
			sys_unlock_mutex(VIDEO_MUTEX);
			sys_sleep(10);
			
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, 0);
			sys_write("-");
			sys_unlock_mutex(VIDEO_MUTEX);
			sys_sleep(10);
			
		}
	}
	
	sys_sleep(1000000);
	total_sleep_period = 0;
	col = 0;
	while (col < 70) {
	
		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, col);
		sys_write(pid_str);
		sys_unlock_mutex(VIDEO_MUTEX);

		sleep_period = (sys_get_rand()/my_pid) % 4 + 1;
		sys_sleep(sleep_period);
		total_sleep_period += sleep_period;
		
		sys_lock_mutex(VIDEO_MUTEX);
		sys_set_cursor(my_pid, col);
		sys_write(".");
		sys_unlock_mutex(VIDEO_MUTEX);
		col++;
	
	}
	sys_exit(total_sleep_period);

}

