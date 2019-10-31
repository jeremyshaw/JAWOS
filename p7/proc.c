// proc.c, 159 JAWOS
// processes are coded here

#include "const-type.h"  // VGA_MASK_VAL
#include "ext-data.h"    // sys_time_count
#include "syscall.h"     // sys service calls
#include "tools.h"       // Number2Str()
#include "proc.h"


void Idle(void) {   // Idle thread, flashing a dot on the upper-left corner
	unsigned short *start_pos = (unsigned short *)0xb8000;
	unsigned short *old;
	while(1) { 
		if (sys_time_count % 100 < 50) {
			if(*start_pos-VGA_MASK_VAL != '*') *old = *start_pos-VGA_MASK_VAL;
			*start_pos = '*' + VGA_MASK_VAL;
		}
		else {
			if((*old) != '*') *start_pos = *old + VGA_MASK_VAL; 
			else *start_pos = ' ' + VGA_MASK_VAL;
		}
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
		else sys_write("prompt: login sucessful!\r");	// break;
		
	}
	
	// sys_vfork(Shell); phase 8
}