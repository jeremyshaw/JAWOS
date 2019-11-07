// proc.c, 159 JAWOS
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "proc.h"


void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner
	unsigned short *start_pos = (unsigned short *)0xb8000;
	char old = '\0';
	while(1) { 
		while (sys_time_count % 200 != 100 || old == '\0') {
			if ((sys_time_count % 200 == 0) && (*start_pos-VGA_MASK_VAL != '*')) {
				old = *start_pos-VGA_MASK_VAL;
				*start_pos = '*' + VGA_MASK_VAL;
			}
		}
		if(*start_pos-VGA_MASK_VAL == '*') *start_pos = old + VGA_MASK_VAL;
		else if( *start_pos != old + VGA_MASK_VAL) old = *start_pos-VGA_MASK_VAL;
		sys_rand_count++;	// part of the "random" counter	
	}
}


void Login(void) {
	
	char login_str[STR_MAX], passwd_str[STR_MAX];
	
	while(1) {
		
		sys_write("login: ");
		sys_read(login_str);
		sys_write("passwd: ");
		sys_read(passwd_str);
		if(StrCmp(login_str, passwd_str) == 0) sys_write("prompt: login failed!\r");
		else {
			sys_write("prompt: login sucessful!\r");	
			// break;	phase 8;
		}
	}
	
	// sys_vfork(Shell); phase 8
}

// Login
   // a successful login calls for the creation of a Shell process
   // via the sys_vfork() call

// Shell
   // loops to prompt for command input and execute:
      // show a prompt (see demo runs)
      // get a command input
      // compare input with one of the valid commands
      // to execute the command via sys_vfork() call
      // on input mismatch a valid-command list is shown

// ShellDir
   // show a faked directory listing

// ShellCal
   // show the calendar of the month

// ShellRoll
   // Roll two dices and call sys_exit() with their sum.
   // To roll a dice is to call sys_get_rand and modulus
   // its return with 6 (for a six-faced dice); and after
   // plus 1 in order to get a number between 1 and 6
   // inclusively.