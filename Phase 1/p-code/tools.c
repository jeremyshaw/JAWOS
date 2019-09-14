// tools.c, 159

//this .c code needs to include spede.h, const-type.h, and ext-data.h
#include "spede.h";
#include "const-type.h";
#include "ext-data.h";


QueEmpty(que_t *p){
// code a QueEmpty() function that checks whether a queue (located by a
if(p->tail ==0) return 1;
// given pointer) is emptyr; returns 1 if yes, or 0 if not

return 0;//not empty
}

QueFull(pointer*){
//similarly, code a QueFull() function to check for being full or not

return 0;//not full
}


code a DeQue() function that dequeues the 1st number in the queue (given
by a pointer); if empty, return constant NONE (-1)

code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB:
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
MemCpy *dst, *src, un int bytes
for i = 0;;;
*dst = *src
dst++;
src++;

