// tools.h prototypes tools in tools.c, 159 
// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _TOOLS_
#define _TOOLS_

#include "const-type.h"

void Number2Str(int x, char *str);
void EnQue(que_t *p, int data);
int DeQue (que_t *p);
void Bzero(char *start, unsigned int bytes);
void MemCpy(char *dst, char *src, unsigned int bytes);
int QueEmpty(que_t *p);
int QueFull(que_t *p);

#endif

