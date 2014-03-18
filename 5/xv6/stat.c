#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"


int
print_stat(char *fileName){
  struct stat st;

  if(stat(fileName, &st) < 0)
    return -1;
  
  printf(1,"file: %s\ninode #: %d\nsize: %d\ndev #: %d\n",
    fileName,
    st.ino,
    st.size,
    st.dev);
  switch(st.type){
    case T_EXTENT:
      printf(1,"type: T_EXTENT\n");
      int i;
      uint addr;
      uint offset;
      uint length;
      for(i = 0; i < NDIRECT; i+=2){
        if (st.addrs[i]){
	addr = st.addrs[i] >> 8;
	offset = st.addrs[i+1];
	length = st.addrs[i] & 0xFF;
	printf(1,"EXTENT\nbase addr: %d\noffset: %d\nlength: %d\n",
	  addr,
	  offset,
	  length);
        }
      }
      break;
    case T_DIR: 
      printf(1,"type: T_DIR\n");
      break;
    case T_FILE:
      printf(1,"type: T_FILE\n");
      for(i = 0; i < NDIRECT; i++)
        printf(1,"addr: %d\n", st.addrs[i]);
      break;
    case T_DEV:
      printf(1,"type: T_DEV\n");
      break;
    default :
      break;
  }
  return 1;
}

int
main(int argc, char *argv[]){
  if(argc != 2){
    printf(1,"usage: stat pathname");    
    exit();
  }
  print_stat(argv[1]);
  exit();
}
