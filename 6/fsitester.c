#include "MFS_Msg.h"
#include "lfsi.h"
#include "mfs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
  assert(fsi_init(argv[1]) == 0); 
  int inum = fsi_lookup(0, ".");
  printf("root inum: %d\n", inum);
  assert(inum >= 0);

  assert(fsi_creat(0, LFS_REGULAR_FILE, "foo") == 0);
  inum = fsi_lookup(0, "foo");
  printf("foo inum: %d\n", inum);
  int rc = fsi_unlink(0, "foo");
  printf("unlink rc: %d\n", rc);
  rc = fsi_lookup(0, "foo");
  printf("lookup rc: %d\n", rc);
}

