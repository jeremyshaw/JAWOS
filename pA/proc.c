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
		if ((sys_time_count % 200 < 100) && (*start_pos-VGA_MASK_VAL != '*')) {
			old = *start_pos-VGA_MASK_VAL;
			*start_pos = '*' + VGA_MASK_VAL;
		} else if(sys_time_count % 200 >= 100) {	// I suppose "else if" is overkill for this
			if(*start_pos-VGA_MASK_VAL == '*') *start_pos = old + VGA_MASK_VAL;
			else if( *start_pos != old + VGA_MASK_VAL) old = *start_pos-VGA_MASK_VAL;		
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
		if(StrCmp(login_str, passwd_str) == 0) sys_write("prompt: login failed!\n\r");
		else {
			sys_write("prompt: login sucessful!\n\r");	
			break;
		}
	}
	sys_vfork(Shell);
	sys_exit(0);
}


void Shell(void) {
	char prompt_str[STR_MAX], p_str[STR_MAX], e_str[STR_MAX];
	int exit_pid, exit_code;
	while(1) {
		sys_write("JAWOS>");
		sys_read(prompt_str);
		if(StrCmp(prompt_str, "dir")) sys_vfork(ShellDir);
		else if (StrCmp(prompt_str, "cal")) sys_vfork(ShellCal);
		else if (StrCmp(prompt_str, "roll")) sys_vfork(ShellRoll);
		else {
			ShellCmds();
			continue;	// replaces double loop with break
		}
		exit_pid = sys_wait(&exit_code);
		Number2Str(exit_pid, p_str);
		sys_write("Exit Pid: ");
		sys_write(p_str);
		sys_write("   ");
		Number2Str(exit_code, e_str);
		sys_write("Exit Code: ");
		sys_write(e_str);
		sys_write("\n\r");
	}
}

void ShellCmds(void) {
	
	sys_write("list of valid commands\n\r");
	sys_write("dir - directory listing\n\r");
	sys_write("cal - fake calendar\n\r");
	sys_write("roll - rolls 2 die\n\r");
	
}
   
void ShellDir(void) {
	
	sys_write(".\n\r");
	sys_write("..\n\r");
	sys_write("folder.exe     awrxgwrxuwrx\n\r");
	sys_write("a.out          a---g---uwrx\n\r");
	sys_exit(0);
	
}


void ShellCal(void) {
	
   sys_write("       MONTH!       \n\r");
   sys_write("Su Mo Tu We Th Fr Sa\n\r");
   sys_write(" 1  2  3  4  5  6  7\n\r");
   sys_write(" 8  9 10 11 12 13 14\n\r");
   sys_write("15 16 17 18 19 20 21\n\r");
   sys_write("22 23 24 25 26 27 28\n\r");
   sys_exit(0);
   
}


void ShellRoll(void) {
	
	int d1, d2;
	
	d1 = sys_get_rand() % 6 + 1;
	sys_sleep(1);	// allows for slightly different rand returns
	d2 = sys_get_rand() % 6 + 1;
	sys_exit(d1 + d2);
	
}