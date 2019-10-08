// proc.h, 159

// Team Name: JAWOS (Members: Alex Leones, Jeremy Shaw, William Guo)

#ifndef _PROC_ //following naming convention from other files
#define _PROC_
//some includes from Phase 0
#include <spede/flames.h>            // some SPEDE stuff that helps...
#include <spede/machine/asmacros.h>
#include <spede/machine/io.h>
#include <spede/machine/pic.h>
#include <spede/machine/proc_reg.h>
#include <spede/machine/seg.h>
#include <spede/stdio.h>

//prototype of Idle() coded in proc.c 
void Idle(void);
void Init(void);
void Many(void);

#endif
