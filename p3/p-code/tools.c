// tools.c, 159
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"

void Number2Str(int x, char str[]) {
	int ni, temp, len; //ni = for loop's i, but for Num2str
	
	temp = x;
	len = 0;
	
	while(temp) {
		temp = temp/10;
		len++;
	}
	temp = x;

	for (ni = 0; ni < len; ni++) {
		//put them in backwards!
		str[len-(ni+1)] = ( temp % 10 ) + '0';
		temp = temp/10;
	}
	str[len] = '\0';
	
}


int QueEmpty(que_t *p) {
	
	if(p->tail == 0) return 1;
	return 0;

}

int QueFull(que_t *p) {
	
	if(p->tail == QUE_MAX) return 1;
	return 0;
	
}


int head;
int j, k, l;

int DeQue(que_t *p){
	
	if(QueEmpty(p) == 1) return NONE;
	else{
		head =  p->que[0];
		for(j = 0; j < p->tail-1; j++){//whoops, needed the -1, lol. Or -- the tail beforehand
			p->que[j] = p->que[j+1];
		}
		p->tail--;
		p->que[p->tail] = NONE; //this is technically better than NULL?
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

void Bzero(char *start, unsigned int max) { for(k = 0; k < max; k++) *start++ = (char) 0; }

void MemCpy(char *dst, char *src, unsigned int max) { for(l = 0; l < max; l++) *dst++ = *src++; }
