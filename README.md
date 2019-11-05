# JAWOS (Jeremy, Alex, & Will Operating System)
CSU Sacramento CSC159/CpE159 Operating System Pragmatics


## What are Phases?
Steps from which the OS is constructed, piece by piece.

### Phase 0
Prep, individual. The one here is Jeremy's.

### Phase 1
Setting up the structure of the OS that we will create.

### Phase 2
Create a second thread that shares CPU time with main thread (IDLE has PID 0, main thread)

### Phase 3
Implemented forking.
```
sys_fork()
```

### Phase 4
mutex added; right now, for video output (text) only

### Phase 5
sys_exit (for child to pass info to parent)
sys_wait (for parent to wait for children ot exit)

### Phase 6
System signalling

### Phase 7
Kernel input "driver" (for reading text from KB)

### Phase 8
Virtual Memory

### Phase 9


## Who?
Jeremy Shaw [jeremyshaw](https://github.com/jeremyshaw)
Alex Leones [aleo101](https://github.com/aleo101)
Will Guo  [williamguo908](https://github.com/willaimguo908)

## Professor
Weide Chang [CSUS](http://athena.ecs.csus.edu/~changw/)
