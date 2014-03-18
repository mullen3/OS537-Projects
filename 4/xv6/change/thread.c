#include "types.h"
#include "mmu.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "fs.h"
#include "proc.h"
#include "x86.h"

// When called by parent, waits for the child threads to complete execution
// by calling the wait() routine.
int
thread_join(void)
{
  return wait();
}


// Allocates a new stack for the child thread and calls clone with the newly 
// allocated stack. The parent returns the pid of the child thread and the
// child calls the routine pointed to by the start_routine argument and passes
// it with the arg argument. Then the child frees the allocated stack and exits
// If allocproc fails, returns -1
int
thread_create(void *(*start_routine)(void*),void *arg){
  void *stack = malloc(4096);  
  int rc = clone(stack, 4096);
  if (rc == 0)
  {
    (*start_routine)(arg);
    free(stack);
    exit();
  }
  else
    return rc;
}

// Initializes a spinlock
void
lock_init(lock_t * lock){
  xchg(&lock->flag, 0);
}

// Acquires a spinlock which returns if no one already has the lock and spins
// if the lock is already acquired.
void
lock_acquire(lock_t * lock)
{
  while(xchg(&lock->flag, 1) == 1)
    ;//spinny spin spins
}

// Releases a spinlock
void
lock_release(lock_t * lock)
{
  xchg(&lock->flag, 0);
}
