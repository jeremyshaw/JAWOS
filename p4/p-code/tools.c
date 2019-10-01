// tools.c, 159
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"


int j, k, l;


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
		str[len-(ni+1)] = ( temp % 10 ) + '0';	// put them in backwards!
		temp = temp/10;
	}
	str[len] = '\0';
	
}


int DeQue(que_t *p){
	
	int head;
	
	if(QueEmpty(p)) return NONE;
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

 
void EnQue(que_t *p, int num){
	
   if(QueFull(p)){
      cons_printf("Panic: queue is full, cannot EnQue!\n");
      breakpoint();
   }else{
      p->que[p->tail] = num;
      p->tail++;  //empty space at end of queue
   }
   
}

int QueEmpty(que_t *p) { (p->tail == 0) ? (return 1) : return 0; }

int QueFull(que_t *p) { (p->tail == QUE_MAX) ? (return 1) : (return 0); }

void Bzero(char *start, unsigned int max) { for(k = 0; k < max; k++) *start++ = (char) 0; }

void MemCpy(char *dst, char *src, unsigned int max) { for(l = 0; l < max; l++) *dst++ = *src++; }
