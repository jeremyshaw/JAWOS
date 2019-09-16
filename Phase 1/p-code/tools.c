// tools.c, 159

//this .c code needs to include spede.h, const-type.h, and ext-data.h
#include "spede.h";
#include "const-type.h";
#include "ext-data.h";


int QueEmpty(que_t *p){
// code a QueEmpty() function that checks whether a queue (located by a
if(p->tail == 0) return 1;
else return -1;
// given pointer) is emptyr; returns 1 if yes, or 0 if not

}

<<<<<<< HEAD
QueFull(que_t *p){ //pointer*?
//similarly, code a QueFull() function to check for being full or not
if(p->tail == QUE_MAX) return 1;
else return 0;//not full
=======
int QueFull(pointer*){
//similarly, code a QueFull() function to check for being full or not
if(p->tail == QUE_MAX) return 1; //full

return 0;//not full
>>>>>>> da4db8c2e83c44f0a02a190fa815f8281b203b27
}


code a DeQue() function that dequeues the 1st number in the queue (given
by a pointer); if empty, return constant NONE (-1)

DeQue(que_t *p, ){
	
	
	
}

//tests if empty, dequeues and pushes elements forward

code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB:

//If index QUE_MAX is not null, cons_printf("Panic... ,
//otherwise searches from 0 to find empty spot, adds to queue.

      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint();

code a Bzero() function to clear a memory region (by filling with NUL
characters), the beginning of the memory location will be given via a
character pointer, and the size of the memory will be given by an unsigned
int 'max'

void Bzero(char *start, unsigned int bytes){
	for(int i = 0, i < bytes, i++){
		*start = '\0'; // *start = (char) 0; in class
		start++;
		//(*start)++
	}
};

code a MemCpy() function to copy a memory region located at a given
character pointer 'dst,' from the starting location at a given character
pointer 'src,' the size will also be given as an unsigned integer 'max'

void MemCpy(*dst, *src, unsigned int bytes){
   for i = 0;;;
   *dst = *src
   dst++;
   src++;
}

