#include <stdio.h>
#include <ctype.h>
#include "libmfs.c"

int main(int argc, char *argv[]) 
{
  if (argc != 3) {
    printf("Usage: client [serverhost] [serverport]");
    exit(1);
  }
  int port = atoi(argv[2]);
  if (isdigit(port)) {
    printf("Error: Parameter [serverport] must be an integer");
    exit(1);
  }
  int rc = MFS_Init(argv[1],port);
  if (rc < 0) {
    printf("Error: Client initialization failed!");
  }
 // printf("Type a the name of the function you would like to call");
 // scanf("%s",&function);
 // function;
  return 1;
}
