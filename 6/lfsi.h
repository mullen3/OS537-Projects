#ifndef __LFSI_h__
#define __LFSI_H__
#include <sys/types.h>
#include "mfs.h"


#define LFS_DIRECTORY    (0)
#define LFS_REGULAR_FILE (1)

#define LFS_BLOCK_SIZE   (4096)
#define SEGMENT_SIZE     (16384)
#define NDIRECT          (14)
#define IMAP_CHUNK_SIZE  (16)

typedef struct __inode_t{
  int size;
  int type;
  int direct[NDIRECT];
}inode_t;

typedef struct __dir_ent_t{
  char name[28]; //28 bytes of a name
  int inum;
}dir_ent_t;

typedef struct __imap_chunk_t{
  int inodes[16];
}imap_chunk_t;

typedef struct __cr_t{
  uint end;
  int chunks[256]; 
}cr_t;

int fsi_init(char *fsi_path);
int fsi_lookup(int pinum, char* name);
int fsi_stat(int inum, MFS_Stat_t *stat);
int fsi_write(int inum, char *buffer, int block);
int fsi_read(int inum, char *buffer, int block);
int fsi_creat(int pinum, int type, char *name);
int fsi_unlink(int pinum, char* name);
int fsi_shutdown();

#endif
