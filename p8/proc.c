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
			break;
		}
	}
	sys_vfork(Shell);
}


void Shell(void) {
	
	char prompt_str[STR_MAX];
	
	while(1) {
	
		sys_write("JAWOS>");
		sys_read(prompt_str);
		if(StrCmp(prompt_str, "Dir")) sys_vfork(ShellDir);
		else if(StrCmp(prompt_str, "Cal")) sys_vfork(ShellCal);
		else if(StrCmp(prompt_str, "Roll")) sys_vfork(ShellRoll);
		else {
			sys_write("list of valid commands\r");
			sys_write("Dir\r");
			sys_write("Cal\r");
			sys_write("Roll\r");
		}
	}
}

   
void ShellDir(void) {
	// show a faked directory listing
	sys_write("dir stuff\r");
	sys_write(".\r");
	sys_write("..\r");
	sys_write("folder.exe     awrxgwrxuwrx\r");	// We may lose points here.
	sys_write("a.out          a---g---uwrx\r");
	sys_exit(0);
	
}


void ShellCal(void) {
   sys_write("       MONTH        \r");
   sys_write(" 1  2  3  4  5  6  7\r");
   sys_write(" 8  9 10 11 12 13 14\r");
   sys_write("15 16 17 18 19 20 21\r");
   sys_write("22 23 24 25 26 27 28\r");
   sys_exit(0);
}


void ShellRoll(void) {
	int d1 = sys_get_rand() % 6 + 1;
	int d2 = sys_get_rand() % 6 + 1;
	sys_exit(d1 + d2);	// you know what the optimization is...
}