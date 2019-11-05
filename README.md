# JAWOS (Jeremy, Alex, & Will Operating System)
CSU Sacramento CSC159/CpE159 Operating System Pragmatics


## What are Phases?
Steps from which the OS is constructed, piece by piece.

### Phase 0
Prep, individual. The one here is Jeremy's.

### Phase 1
Setting up the structure of the OS that we will create.

### Phase 2
Create a second process that shares CPU time
```
Idle() - system Idle process - PID 0
Init() - new process
```

### Phase 3
Implemented forking.
```
int sys_fork() - returns child pid to parent, 0 to child
```

### Phase 4
mutex added; right now, for video output (text) only
```
sys_unlock_mutex(mutex ID)
sys_lock_mutex(mutex ID)
```

### Phase 5
```
sys_exit (for child to pass info to parent)
sys_wait (parent waiting for children to exit)
```

### Phase 6
System signalling. From midterm, a way somewhat synchronize processes.
```
sys_signal()
sys_kill()
```

### Phase 7
Kernel input "driver" (for reading text from KB)
```
sys_read()
```

### Phase 8
Virtual Memory - in progress as of 5NOV19

### Phase 9


## Who?
* Jeremy Shaw [jeremyshaw](https://github.com/jeremyshaw)
* Alex Leones [aleo101](https://github.com/aleo101)
* Will Guo  [williamguo908](https://github.com/willaimguo908)

## Professor
Weide Chang [CSUS](http://athena.ecs.csus.edu/~changw/)
