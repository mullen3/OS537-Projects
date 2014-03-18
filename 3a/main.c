#include "mem.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
extern void printList();

int main() {
   assert(Mem_Init(1024, 0) == 0);
   void * ptr[10]; 
   
   ptr[0] = Mem_Alloc(4);
   ptr[1] = Mem_Alloc(38);
   ptr[2] = Mem_Alloc(40);
   ptr[3] = Mem_Alloc(120);
   ptr[4] = Mem_Alloc(12);
   ptr[5] = Mem_Alloc(18);
   ptr[6] = Mem_Alloc(73);
   ptr[7] = Mem_Alloc(14);
   ptr[8] = Mem_Alloc(12);
   ptr[9] = Mem_Alloc(504);
   printList();   
   (void) Mem_Free(ptr[0]);
   printList();
   (void) Mem_Free(ptr[2]);
   printList();
   (void) Mem_Free(ptr[4]);
   printList();
   (void) Mem_Free(ptr[7]);
   printList();
   (void) Mem_Free(ptr[8]);
   printList();
   (void) Mem_Free(ptr[1]);
   printList();
   (void) Mem_Free(ptr[5]);
   printList();
   (void) Mem_Free(ptr[3]);
   printList();
   (void) Mem_Free(ptr[6]);
   printList();
   ptr[6] = Mem_Alloc(888);
   printf("%p\n", ptr[6]);
   printList();
   (void) Mem_Free(ptr[6]);
   printList();
   exit(0);
}
