# JAWOS (Jeremy, Alex, & Will Operating System)
CSU Sacramento CSC159/CpE159 Operating System Pragmatics
Fall 2019

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
StrCmp()
```

### Phase 8
Virtual Memory
```
sys_vfork(); - more like a v-spawnSR with some basis on a "simple" fork
```

### Phase 9
TTY (simple, one way)
```
TTYSR();
```

### Phase A
Will we have one? I'd guess it would be a bidirectional TTY

## Who?
* Jeremy Shaw [jeremyshaw](https://github.com/jeremyshaw)
* Alex Leones [aleo101](https://github.com/aleo101)
* Will Guo  [williamguo908](https://github.com/williamguo908)

## Professor
Weide Chang [CSUS](http://athena.ecs.csus.edu/~changw/)
