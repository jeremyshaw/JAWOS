Sacramento State, Computer Engineering / Computer Science
CpE/CSc 159 Operating System Pragmatics (Fall 2019)
Instructor: W. Chang

OS Kernel Programming Phase 7
Device Driver: Busy Polls (Console Keyboard)

The Goal

We have experienced a simple method for the console video output
(as the STDOUT channel of processes) with our kernel sys_write
service. To learn more about how device drivers are created, here
we look into a way to get the input from the console keyboard input
(as the STDIN channel of processes).

Since the kernel is frequently running in the background, it can
be a good candidate to constantly poll the console keyboard for
any input and queue each input character to a string buffer for
processes to retrieve via sys_read calls.

A keyboard service routine (KBSR) should be performed after the
kernel has performed a specific service (for a specific event).
The KBSR check for any keyboard input. If so, it reads the character
in and checks if there is any process waiting for it (in a special
wait queue). If no process awaits, the character is queued (in a
special buffer). If there is a waiting process, the process will
be released and receive the character.

A process calls sys_read() providing it an address of a character
string for it to fetch. The sys_read() in terms performs a loop to
issue an inline assembly 'int ?' call to get a character returned
at a time until getting a RETURN key, then the loop ends and the
string is formed to return to the process.

As sys_read() is actually served by SysRead() in the kernel, it
dequeues the character buffer a character for sys_read(). However,
if there is no character left in the buffer, the process will be
preempted: enqueued in a wait queue and its state becomes IO_WAIT
(and run_pid is no longer).

The above description is similar to the wait-exit or mutex operation.
We will need a special keyboard data structure (kb_t kb) to host its
buffer and wait queue (kb_t: que_t buffer, wait_que).

What Are to Be Added:

1. We need a new service name, a new constant, and a new state:
      SYS_READ=142, STR_MAX=20, and IO_WAIT
   plus, a new structure kb_t that has two queues: buffer and wait_que.

2. We need a new kernel data 'kb' of the 'kb_t' type.
   It is 1st cleared during the OS bootstrap.

3. In main(), SpawnSR(Login) instead of Init.

4. In Kernel(), delete: if(cons_kbhit)... (all code about keyboard)
   and replace with a call to KBSR() where all keyboard things are
   taken care of.

5. a new Login() in proc.c:
   void Login(void) {
      char login_str[STR_MAX], passwd_str[STR_MAX];

      while(1) {
         prompt for login entry
         read keyboard into login_str
         prompt for password entry
         read keyboard into login_str

         call StrCmp() to compare the 2 strings 
         if same -> prompt: login successful!\r  // & break; in next phase
         else -> prompt: login failed!\r
      }
//      sys_vfork(Shell);  // virtual-fork Shell (virtual memeory) phase8
   }

6. add a new syscall sys_read():
   void sys_read(char *str) {  // the string
      ...
      Apply a loop for the following logic:
         Call asm("... issue an 'int ?' to do SYS_READ);
         and get a character at a time.

         'Echo' back the character to the console video:
         build a small string with it and call sys_write().

         Add the character to the string until it is the RETURN key
         ('\r') in which a null character ('\0') is added instead.

         The whole string should not exceed STR_MAX bytes. (After
         getting STR_MAX-1 characters, append NUL and return.)

7. add a new kernel service routine SysRead():
   if the buffer in the kb data structure has characters (not empty):
      get the 1st character and give it to the running process
   else
      queue the running process ID to the wait queue of the kb data structure
      change the process state to IO_WAIT
      no running process now

8. add a new kernel service routine KBSR():
   if keyboard is not even pressed: return
   read in the key
   if the key is '$,' breakpoint() // used to be 'b'

   if no process awaits (wait queue is empty in kb):
      enqueue the key to the buffer of kb
   else
      release the 1st process from the wait queue
      alter its state, queue it to the new queue
      give it the key
   
9. modify kernel service routine SysWrite():
   When the character to display is '\r,' instead of display it, advance
   sys_cursor to the start position of the next new row.
   When sys_cursor is wrapping back to VIDEO_START, erase the whole screen.

