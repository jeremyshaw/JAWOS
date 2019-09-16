// proc.c, 159
// processes are coded here



//need to include const-type.h and ext-data.h
#include "const-type.h";
#include "ext-data.h";
// Code an Idle() function that doesn't have any input or return, but just
// flickers the dot symbol at the upper-left corner of the target PC display.

// It reads sys_time_count and at each second interval writes '.' or ' ' via
// an unsigned short pointer to the VGA video memory location 0xb8000.
// Apply the VGA_MASK_VAL when showing . or space so it will appear to be
// boldface writing.
    // ... declare a pointer and set it to the upper-left display corner ...
    // ... declare and set a flag ...
    // in an infinite loop:
       // whenever the system time reaches a multiple of 100 (per 1 second):
          // a. judging from the flag and show either the dot or space
          // b. alternate the flag

void Idle(void){
	int flag = 0; //flat = 1 = display
	if(sys_time_count % 100 == 0){
		flag = 1;
		if(flag==1){
			display '.';
			flag = 1;
		}
		else{
			display ' ';
			flag = 0;
		}
	}
	else if(sys_time_count %200 == 0){
		
		//sys_time_count = 0; //make sure this doesn't cause issues outside of here!
	}
	
	
};
