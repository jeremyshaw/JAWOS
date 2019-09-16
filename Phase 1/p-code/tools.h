// tools.h, 159
// prototypes tools in tools.c

#ifndef _TOOLS_
#define _TOOLS_

#include "..." // need definition of 'que_t' below

...
void EnQUe(int data, que_t *p);

int deque (que_t *p);

void Bzero(char *start, unsigned int bytes);

void MemCpy(*dst, *src, unsigned int bytes);

#endif

