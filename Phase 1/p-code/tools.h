// tools.h, 159
// prototypes tools in tools.c

#ifndef _TOOLS_
#define _TOOLS_

#include "..." // need definition of 'que_t' below

...
void EnQue(int data, que_t *p);

int DeQue (que_t *p);

void Bzero(char *start, unsigned int bytes);

void MemCpy(*dst, *src, unsigned int bytes);

int QueEmpty(que_t *p);

int QueFull(que_t *p);

#endif

