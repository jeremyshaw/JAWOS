// tools.c, 159
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#include "spede.h"
#include "const-type.h"
#include "ext-data.h"
#include "tools.h"


int k, l;


int StrCmp (char *a, char *b){
	// classic strcmp would return difference, iirc. But all we need is 1 for sucess, 0 for fail
	sys_write("a");
	sys_write(a);
	sys_write("b");
	sys_write(b);
	while(1) {
		if(*a == '\0' && *b == '\0') return 1; 
		if(*b != *a) break;
		a++;
		b++;
	}

	return 0;
	
}


void Number2Str(int x, char str[]) {
	
	int ni, temp, len;
	
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
	
	int head, i;
	
	if(QueEmpty(p)) return NONE;
	else{
		head =  p->que[0];
		for(i = 0; i < p->tail-1; i++){
			p->que[i] = p->que[i+1];
		}
		p->tail--;
		p->que[p->tail] = NONE;
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

int QueEmpty(que_t *p) { 
	if (p->tail == 0) return 1;
	return 0;
}

int QueFull(que_t *p) { 
	if (p->tail == QUE_MAX) return 1;
	return 0; 
}

void Bzero(char *start, unsigned int max) { for(k = 0; k < max; k++) *start++ = (char) 0; }

void MemCpy(char *dst, char *src, unsigned int max) { for(l = 0; l < max; l++) *dst++ = *src++; }
