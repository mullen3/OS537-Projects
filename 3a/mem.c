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
//struct Node *headUsed = NULL;
int init = 1;

int Mem_Init(int sizeOfRegion,int debug)
{
  if (sizeOfRegion <= 0 || init == 0) { 
    m_error = E_BAD_ARGS;
    return -1;
  }
  sizeOfRegion += getpagesize() - (sizeOfRegion % getpagesize());
  struct Node *freeNode;
  int fd = open("/dev/zero/", O_RDWR);
  void *ptr = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_ANON|MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED) 
  {
    perror("mmap"); 
    exit(1);
  }
  freeNode = (Node *)ptr;
  freeNode.size = sizeOfRegion;
  freeNode.next = NULL;
  headFree = freeNode;
  close(fd);
  init = 0;
  return 0;
}

void *chunkNode(struct Node *node, int size) {
  struct Node *newNode;
  newNode = node;
  size += size - size % 8 + 8;
  int sizeOfRegion = node->size;
  struct Node *next = node->next;
  node += size;
  node->size = sizeOfRegion - size;
  node->next = next;


  newNode->size = size;
  newNode->next = NULL;

  return newNode + 8;
}

void *getWorstFit(int size)
{
  struct Node *worst = NULL;
  struct Node *node = headFree;
  while (node != NULL)
  {
    if (worst == NULL || node->size > worst->size)
      worst = node;

    node = node->next;
  }
  return (worst->size < size) ? worst : NULL;
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
  struct node *prev = NULL;
  while (curr != NULL) {
    //
    if (node == curr + curr->size)
    {   
      curr->size += node->size;
      node = curr;

      curr = curr->next;
      while (curr!=NULL)
      {
	if (node + node->size == curr)
	{
	   node->size += curr->size;
	   prev->next = curr->next;
           return node;
	}
	 prev = curr;
	 curr = curr->next;
       }
      return node;
    }
    else if (curr == node + node->size)
    {
      node->size += curr->size;
      node->next = curr->next;
      prev->next = node;

      curr = curr->next;
      while (curr!=NULL)
      {
	if (curr + curr->size == node)
	{
	  curr->size += node->size;
	  curr->next = node->next;
          return node;
	}
	prev = curr;
	curr = curr->next;
      }
      return node;
    }
    prev = curr;
    curr = curr->next;
  }
  node->next = headFree;
  headFree = node;
  return node;
}


int Mem_Free(void *ptr)
{
  if (ptr == NULL) return -1;
  struct Node *node = (Node *) ptr - 8;
  node = findNeighborNode(node);
  return 0;
}

void Mem_Dump()
{

}
