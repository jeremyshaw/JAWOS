// tools.c, 159

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

//includes from p1
#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"

void Number2Str(int x, char *str) {
	
	int len = 1; //needs an extra boost for the "reversing" loop
	char *str2;
	while(x>10){
		int temp;
		temp = x % 10;
		temp += '0'; //convert to ascii digits
		*str2 = temp;
		str2++;		
		len++;
	}
	*str2 = x;
	
	//now, need to reverse the string to actually make it in the correct order. Don't forget to add a NULL.
	while(len){
		*str = &str2;
		str++;
		str2--;
	}
	*str = (char)0;
}


//all of this is from p1

int QueEmpty(que_t *p){
// code a QueEmpty() function that checks whether a queue (located by a
if(p->tail == 0) return 1;
else return 0;
// given pointer) is emptyr; returns 1 if yes, or 0 if not
}

int QueFull(que_t *p){ //pointer*?
//similarly, code a QueFull() function to check for being full or not
if(p->tail == QUE_MAX) return 1;
else return 0;//not full - don't need the else, either
}


// code a DeQue() function that dequeues the 1st number in the queue (given
// by a pointer); if empty, return constant NONE (-1)
int head;
int j, k, l;

int DeQue(que_t *p){
	if(QueEmpty(p) == 1){
		return NONE; //NONE = -1
	}
	else{
		head =  p->que[0];
		for(j = 0; j < p->tail-1; j++){//whoops, needed the -1, lol. Or -- the tail beforehand
			p->que[j] = p->que[j+1];
		}
		p->tail--;
		p->que[p->tail] = -1; //this is technically better than NULL?
		return head;
	}
}

//tests if empty, dequeues and pushes elements forward

/* code an EnQue() function given a number and a queue (by a pointer), it
appends the number to the tail of the queue, or shows an error message
and go into the GDB */
      
void EnQue(que_t *p, int num){
   if(QueFull(p) == 1){
      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint();
   }else{
      p->que[p->tail] = num;
      p->tail++;  //empty space at end of queue
   }
}

//If index QUE_MAX is not null, cons_printf("Panic... ,
//otherwise searches from 0 to find empty spot, adds to queue.

// code a Bzero() function to clear a memory region (by filling with NUL
// characters), the beginning of the memory location will be given via a
// character pointer, and the size of the memory will be given by an unsigned
// int 'max'

void Bzero(char *start, unsigned int max){
	for(k = 0; k < max; k++){
		*start = (char) 0; // *start = (char) 0; in class
		start++;
	}
}

// code a MemCpy() function to copy a memory region located at a given
// character pointer 'dst,' from the starting location at a given character
// pointer 'src,' the size will also be given as an unsigned integer 'max'

void MemCpy(char *dst, char *src, unsigned int max){
	for(l = 0; l < max; l++){
	    *dst = *src;
		dst++;
		src++;
	}
}
