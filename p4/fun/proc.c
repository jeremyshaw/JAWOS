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


void Init(void) {    // illustrates a racing condition
	
	int col, my_pid, counter, forked_pid, rand;
	char pid_str[CHR_ARY];
	
	counter = 3;
	while(counter--){
		forked_pid = sys_fork();
		if(forked_pid == NONE) sys_write("sys_fork() failed!\n");
	}
	
	my_pid = sys_get_pid();              // what's my PID
	Number2Str(my_pid, pid_str);         // convert # to str
	
	while(1){
		
		for (col = 0; col < 70; col++) {
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(my_pid, col);
			sys_write(pid_str);
			sys_unlock_mutex(VIDEO_MUTEX);
			rand = sys_get_rand() / my_pid;
			sys_sleep(rand % 4 + 1);
		}
		
		sys_lock_mutex(VIDEO_MUTEX);
		for (col = 0; col < 70; col++) {
			sys_set_cursor(my_pid, col);
			sys_write(" ");
		}
		sys_unlock_mutex(VIDEO_MUTEX);
		
		sys_sleep(30);	// sleep for 30 (3 seconds)
		
	}
	
}

void Many(void) {    // illustrates a racing condition
	
	int row, my_pid, counter, forked_pid, rand, randC, randR, randE;
	char pid_str[CHR_ARY], str[2];
	
	counter = 4;
	while(counter--){
		forked_pid = sys_fork();
		if(forked_pid == NONE) sys_write("sys_fork() failed!\n");
	}
	
	my_pid = sys_get_pid();              // what's my PID
	Number2Str(my_pid, pid_str);         // convert # to str
	sys_sleep(2);
	sys_sleep(sys_get_rand()/my_pid % 30);
	
	while(1){
		
		randR = sys_get_rand()/my_pid % 15;	// starting point, going down
		randC = sys_get_rand()/my_pid % 80;	// starting point, side to side
		rand = sys_get_rand()/my_pid % 6 + 1;	// speed
		randE = sys_get_rand()/my_pid % 10 + 10;	// random endpoint
		str[0] = (unsigned)(sys_get_rand()/my_pid % 119 + 3);
		if(str[0] == 32 || str[0] == 95) str[0]++;
		str[1] = 0;
		
		for (row = randR; row < 25 && row - randR < randE; row++) {
			sys_lock_mutex(VIDEO_MUTEX);
			sys_set_cursor(row, randC);
			if(row <= 25 && ((row - randR) == (randE-1)) ) {
				sys_write_decay(str);
			} else {
				sys_write(str);
			}
			if(randR < row) {
				sys_set_cursor(row-1, randC);
				sys_write_decay(str);
			}
			sys_unlock_mutex(VIDEO_MUTEX);
			sys_sleep(rand);
		}
		
		sys_sleep(30);	// sleep for 30 (3 seconds)
		
	}
	
}


