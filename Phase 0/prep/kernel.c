//*******************************************************************
// NAME: Jeremy Shaw
// kernel.c
// Phase 0, Prep 4, Timer Event Handling
//*******************************************************************

#include <spede/flames.h>            // some SPEDE stuff that helps...
#include <spede/machine/asmacros.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/stdio.h>

#define TIMER_EVENT 32         // timer interrupt event code
#define PIC_MASK_REG 0x21      // I/O loc # of PIC mask
#define PIC_MASK_VAL ~0x01     // mask code for PIC
#define PIC_CONT_REG 0x20      // I/O loc # of PIc control
#define TIMER_SERVED_VAL 0x60  // control code sent to PIC
#define VGA_MASK_VAL 0x0f00    // bold face, white on black


void TimerEntry(void);


unsigned int time_count = 0;
unsigned short *center_pos = (unsigned short *)0xb8000 + 12 * 80 + 40;
struct i386_gate *idt;         // interrupt descriptor table


// prototype a void-return void-argument TimerEntry function:
//....                           // actually coded in entry.S
void TimerService(void){
	outportb(PIC_CONT_REG, TIMER_SERVED_VAL);
	
	time_count += 1;
	
	if(time_count==100){
		*center_pos = 'W' + VGA_MASK_VAL;
		*(center_pos + 1) = 'C' + VGA_MASK_VAL;
	}
	else if(time_count==200){
		*center_pos = ' ' + VGA_MASK_VAL;
		*(center_pos + 1) = ' ' + VGA_MASK_VAL;
		time_count = 0;
	}
	
	return;
	
}

// Program a C function 'void TimerService(void)'
   // * first, notify PIC the event is served: outportb(PIC_CONT_REG, TIMER_SERVED_VAL);

   // * upcount time_count by 1

   // * as each second arrives: print/erase initials of your name on the center of the
   // screen, so it gives the effect of flashing out the initials at the rate of 2Hz
   // (i.e., 1 second on, 1 second off, see demo run), use time_count, center_pos and
   // VGA_MASK_VAL to achieve this

   // * return
   
int main(void){
	char ch;
	idt = get_idt_base();
	fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_MASK_REG, PIC_MASK_VAL);
	
	asm("sti");
	while(1){
		if(cons_kbhit()){
			ch = cons_getchar();
			cons_printf(" pressed");
			if(ch=='x')breakpount();
			
		}
		
	}
	
	return 0;
	
}

// Program a C function 'int main(void)'
   // * declare local char 'ch' DONE

   // * set 'idt' to the return of 'get_idt_base()' call DONE

   // * set an entry in idt to handle future events of timer interrupts: DONE
      // fill_gate(&idt[TIMER_EVENT], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);

   // * send PIC_MASK_VAL to PIC_MASK_REG using outportb() call like above DONE

   // * enable the CPU to handle interrupts using inline assembly macro: 'asm("sti");' DONE

   // * perform an infinite loop that repeats:
        // check the keyboard of the target PC, if it has just been pressed:
           // a. read the key into ch (see example.c)
           // b. show a message onto the target PC (see demo run)
           // c. if ch is 'x' (for exit) break loop: 'break;'

   // * return 0

