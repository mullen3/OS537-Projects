#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "param.h"

lock_t *lock;
int counter;

// Increments a counter by one in a for-loop argument a number of times
void *
counter_inc(void * a)
{ 
  int i;   
  for(i = 0; i < (int)a; i++) 
  { 
    lock_acquire(lock);
    counter++;  
    lock_release(lock);
  }
  return NULL;
}

void
usage()
{
  printf(1,"Usage: threadtest numThreads loopCount [outerLoopCount]\n");
  exit();
}


int
main(int argc, char *argv[])
{
  if(argc != 3 && argc != 4) usage();
  int numThreads = atoi(argv[1]);
  int loopCount = atoi(argv[2]);

  // optional testing parameter for looping the entire process
  int outerLoopCount = 1;
  if(argc == 4) outerLoopCount = atoi(argv[3]);
  
  if (numThreads > NPROC - 3)
  {
    printf(2, "Error: Max number of threads is 61(NPROC - kernel, shell, this process)\n");
    exit();
  }
  counter = 0;
  lock = malloc(sizeof(lock_t));
  lock_init(lock);
  
  int j;
  for (j = 0; j < outerLoopCount; j++)
  {
    int i;
    for (i = 0; i < numThreads; i++)
      if (thread_create(counter_inc,(void *)loopCount) == -1)
      {
        printf(2, "Error: thread_create failure");
        exit();
      }

    for(; i > 0; i--)
      thread_join(); 
  }
  
  // make sure result is correct
  if(counter != numThreads * loopCount * outerLoopCount)
  {
    printf(1,"Error: counter != numThreads * loopCount [* outerLoopCount(optional)]\n");
    exit();
  }
  
  printf(1,"Counter: %d\n", counter);
  exit();
}
