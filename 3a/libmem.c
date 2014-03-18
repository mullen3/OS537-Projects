#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "mem.h"


int m_error;
struct Node
{
  int size;
  struct Node *next;
};

struct Node *headFree = NULL;
struct Node *listPrev = NULL;
int init = 1;

int Mem_Init(int sizeOfRegion,int debug)
{
  if (sizeOfRegion <= 0 || init == 0) { 
    m_error = E_BAD_ARGS;
    return -1;
  }
  if (sizeOfRegion % getpagesize() != 0) 
    sizeOfRegion += sizeOfRegion - (sizeOfRegion % getpagesize());
  struct Node *freeNode;
  int fd = open("/dev/zero/", O_RDWR);
  void *ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED) 
  {
    perror("mmap"); 
    exit(1);
  }
  freeNode = (struct Node *)ptr;
  freeNode->size = sizeOfRegion;
  freeNode->next = NULL;
  headFree = freeNode;
  close(fd);
  init = 0;
  return 0;
}

struct Node *addSizeToNode(struct Node *node, int val)
{
  void *ptr =(void *) node;
  ptr += val;
  return (struct Node *) ptr;
}

void *chunkNode(struct Node *node, int size) {
  struct Node *newNode;
  newNode = node;
  if (size % 8 == 0) size += sizeof(struct Node);
  else{
   int i = size/8;
   size = (i+1)*8 + sizeof(struct Node);
  }

  int sizeOfRegion = node->size;

  struct Node *next = node->next;
  node = addSizeToNode(node, size);
  node->size = sizeOfRegion - size;
  if (node->size == 0 && listPrev != NULL) 
  {
    listPrev->next = next;
  }
  else if (node->size == 0 && listPrev == NULL)
  {
    headFree = NULL;
  }
  else
  {
    if(listPrev != NULL)listPrev->next = node;
    node->next = next;
  }
  if (headFree == newNode)
    headFree = node;

  newNode->size = size;
  newNode->next = NULL;

  return (void *) newNode + sizeof(struct Node);
}

void *getWorstFit(int size)
{
  struct Node *worst = NULL;
  struct Node *node = headFree;
  struct Node *prev = NULL;
  while (node != NULL)
  {
  //  printf("infinite while");
    if (worst == NULL || node->size > worst->size)
     {
       worst = node;
       listPrev = prev;
     }
    prev = node;
    node = node->next;
  }
  return (worst->size >= size + sizeof(struct Node)) ? worst : NULL;
}

void *Mem_Alloc(int size)
{
  struct Node *worstFit = getWorstFit(size);
  if (worstFit == NULL) { 
     m_error = E_NO_SPACE;
     return NULL;
  }
  return chunkNode(worstFit,size);
}


void *findNeighborNode(struct Node *node)
{
  struct Node *curr = headFree;
  struct Node *prev = NULL;
  int coalesced = 0;

  while (curr != NULL) {
    
    //node at end of free list element
    if (node == addSizeToNode(curr, curr->size))
    {
      curr->size += node->size;
      
      //take out of free list and add to head
      if (!coalesced)
      {
        if (curr != headFree)
	{
           prev->next = curr->next;
	   curr->next = headFree;
	   headFree = curr;
	   node = curr;
	   curr = prev;
	}
	else
	{
           node = curr;
	}
        coalesced = 1;
      }
      else
      {

	prev->next = curr->next;
	curr->next = node->next;

	headFree = curr;
	return curr;
      }


    }

    //node at beginning of free list element
    else if (curr == addSizeToNode(node, node->size))
    {
      //add free list element to node
      node->size += curr->size;

      
      //take free list element out of list and add new element at head
      if (!coalesced)
      {
        if (curr != headFree)
	{
          prev->next = curr->next;
	  node->next = headFree;
	  curr = prev;
	  headFree = node;
	}
        else
	{
          node->next = curr->next;
	  curr = node;
	  headFree = node;
	}
	coalesced = 1;
      }
      else
      {
        prev->next = curr->next;
	return node;
      }
    }     
    
    prev = curr;
    curr = curr->next;
  }
  //if not coalesced with any elements, add to head of list
  if(!coalesced){
    node->next = headFree;
    headFree = node;
    }
  return node;
}


int Mem_Free(void *ptr)
{
  if (ptr == NULL) return 0;
  ptr -= sizeof(struct Node);
  struct Node *node = (struct Node *) ptr;
  node = findNeighborNode(node);
  return 0;
}

void Mem_Dump()
{

}

void printList()
{
  int i = 0;
  struct Node * curr = headFree;
  while(curr != NULL)
  {
     printf("list member:%p\nlist index:%d\nlist size:%d\n", (void *) curr,
     i, curr->size);
     curr=curr->next;
     i++;
  }
}
