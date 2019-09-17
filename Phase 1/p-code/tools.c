// tools.c, 159

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

//this .c code needs to include spede.h, const-type.h, and ext-data.h
#include "spede.h"
#include "const-type.h"
#include "ext-data.h"


int QueEmpty(que_t *p){
// code a QueEmpty() function that checks whether a queue (located by a
if(p->tail == 0) return 1;
else return 0;
// given pointer) is emptyr; returns 1 if yes, or 0 if not
}


int QueFull(que_t *p){ //pointer*?
//similarly, code a QueFull() function to check for being full or not
if(p->tail == QUE_MAX) return 1;
else return 0;//not full
}


// code a DeQue() function that dequeues the 1st number in the queue (given
// by a pointer); if empty, return constant NONE (-1)

int DeQue(que_t *p){
	if(QueEmpty(*p) == 1){
		return -1;
	}
	else{
		int head =  p->que[0];
		for(int i = 0; i<p->tail; i++){
			que[i] = que[i+1];
		}
		que[tail] = '\0';
		return head;
	}
}

//tests if empty, dequeues and pushes elements forward

/* code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB:
      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint(); */
      
EnQue(que_t *p, int num){
   if(QueFull(*p) == 1){
      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint();
   } else {
      p-> *tail = num;
      tail++;  //empty space at end of queue
   }
}

//If index QUE_MAX is not null, cons_printf("Panic... ,
//otherwise searches from 0 to find empty spot, adds to queue.

      //cons_printf("Panic: queue is full, cannot EnQue!\n");
     // breakpoint();

// code a Bzero() function to clear a memory region (by filling with NUL
// characters), the beginning of the memory location will be given via a
// character pointer, and the size of the memory will be given by an unsigned
// int 'max'

void Bzero(char *start, unsigned int max){
	for(int i = 0, i < max, i++){
		*start = '\0'; // *start = (char) 0; in class
		start++;
		//(*start)++
	}
};

// code a MemCpy() function to copy a memory region located at a given
// character pointer 'dst,' from the starting location at a given character
// pointer 'src,' the size will also be given as an unsigned integer 'max'

void MemCpy(*dst, *src, unsigned int max){
	for(int i = 0, i<max, i++){
	   *dst = *src
		dst++;
		src++;
	}
}

