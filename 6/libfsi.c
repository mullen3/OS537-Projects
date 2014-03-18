#include "udp.h"
#include "MFS_Msg.h"
#include "lfsi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int fsid;
cr_t cr;
int inodes[4096];

int get_inode(int inum, inode_t *node)
{ 
  printf("inum: %d, inodes[inum]: %d\n", inum, inodes[inum]);
  if (inum < 0 || inodes[inum] == -1) return -1;
  lseek(fsid, inodes[inum], SEEK_SET);
  int rc = read(fsid,node, sizeof(inode_t));
  printf("get_inode rc:%d, inode type:%d\n", rc, node->type);
  return rc;
}

int get_block(inode_t *inode, char* buffer, int block)
{
  printf("%d\n", block);
  if (block > 13) return -1;
  lseek(fsid, inode->direct[block], SEEK_SET);
  int rc = read(fsid, buffer, LFS_BLOCK_SIZE);
  printf("read rc: %d\n", rc);
  return rc;
}

int fsi_build(char *fsi_path){
  int fd = open(fsi_path,O_CREAT|O_RDWR, S_IRWXU);
  if (fd == -1)
    return -1;

  //add shit
  dir_ent_t dir_block[128];
  strcpy(dir_block[0].name, ".");
  dir_block[0].inum = 0;
  strcpy(dir_block[1].name, "..");
  dir_block[1].inum = 0;

  //set shit to -1
  int i = 2;
  for (;i < 128; i++)
    dir_block[i].inum = -1;
 
  int j;
  for(j = 0; j < 128; j++)
    if(dir_block[j].inum != -1)
      printf("name: %s, inum: %d\n", dir_block[j].name, dir_block[j].inum);
  //write dir block
  int pos = 0;
  lseek(fd, sizeof(cr_t), SEEK_SET);
  write(fd, dir_block, 128*sizeof(dir_ent_t));
  

  //create root
  inode_t root;
  root.type = LFS_DIRECTORY;
  root.size = LFS_BLOCK_SIZE;
  root.direct[0] = pos + sizeof(cr_t);
  for (i = 1; i < NDIRECT; i++) root.direct[i] = -1;

  //write root
  pos += 128*sizeof(dir_ent_t);
  write(fd, &root, sizeof(inode_t));
  inodes[0] = pos + sizeof(cr_t);

  for(i = 1; i < 4096; i++)
    inodes[i] = -1;
  
  //write inode map chunk
  pos += sizeof(inode_t);
  cr.chunks[0] = pos + sizeof(cr_t);
  
  for (i = 1; i < 256; i++)
    cr.chunks[i] = -1;

  write(fd, inodes, 16*sizeof(int));
  pos += 16*sizeof(int);
  cr.end = pos + sizeof(cr_t);
 
  lseek(fd, 0, SEEK_SET);
  write(fd, &cr, sizeof(cr_t));
  fsync(fd);
  return fd;
}

int fsi_init(char *fsi_path){
  int fd = open(fsi_path,O_RDWR);
  if (fd == -1){
    if (errno == ENOENT)
      fd = fsi_build(fsi_path);
    else
      return -1;
  }
  // load existing file image
  else {
    lseek(fd, 0, SEEK_SET);
    int rc = read(fd, &cr, sizeof(cr_t));
    assert(rc == sizeof(cr_t));
    int i;
    for (i = 0; i < 256; i++){
      if (cr.chunks[i] != -1){
        lseek(fd, cr.chunks[i], SEEK_SET);
        rc = read(fd, &inodes[i*16], sizeof(imap_chunk_t));
        assert(rc != -1);
      }
      else{
        int j;
	for (j = 0; j < 16; j++)
	  inodes[i*16 + j] = -1;
      }
    }
  }
  fsid = fd;
  return 0;
}

int fsi_lookup(int pinum, char* name)
{
  inode_t node;
  if (get_inode(pinum, &node) < 0) return -1;
  printf("node type: %d\n", node.type);
  if (node.type != LFS_DIRECTORY) return -1; 

  int block = 0;
  dir_ent_t dir_block[128];
  
  int done = 0;
  while(!done && block < NDIRECT){
    int rc = get_block(&node, (char *) &dir_block, block);
    if (rc == -1) return -1;
    
    int i;
    for(i = 0; i < 128; i++)
    {
      printf("dir ent: %s, name: %s\n", dir_block[i].name, name);
      
      if (strcmp(dir_block[i].name, name) == 0)
        return dir_block[i].inum;
    }
    block++;
  }
  return -1;
}

int fsi_stat(int inum, MFS_Stat_t *stat)
{
  inode_t node;
  if (get_inode(inum, &node) < 0) return -1;
  stat->type = node.type;
  stat->size = node.size;
  return 0; 
}

int fsi_write(int inum, char *buffer, int block)
{
  inode_t node;
  if (get_inode(inum, &node) < 0) return -1;
  // has to be regular file
  if (node.type != LFS_REGULAR_FILE) return -1;
  // within range of valid blocks
  if (block > 13) return -1;
  
  printf("write after checks\n");

  lseek(fsid, cr.end, SEEK_SET);
  int pos = cr.end;
  
  printf("before writing, lseeked to: %d\n", cr.end);
  if(write(fsid, buffer, LFS_BLOCK_SIZE) < 0) return -1;

  pos += LFS_BLOCK_SIZE;
  
  if(node.direct[block] == -1 && node.size < block * LFS_BLOCK_SIZE)
    node.size = block * LFS_BLOCK_SIZE;
  
  printf("wrote buffer\n");  
  // record new inode position
  inodes[inum] = pos;
  node.direct[block] = cr.end;

  if(write(fsid,&node, sizeof(inode_t)) < 0) return -1;
  pos += sizeof(inode_t);
  
  cr.chunks[inum/IMAP_CHUNK_SIZE] = pos;
  if(write(fsid,&inodes[inum/IMAP_CHUNK_SIZE*IMAP_CHUNK_SIZE], IMAP_CHUNK_SIZE *sizeof(int)) < 0) 
    return -1;
  
  pos += IMAP_CHUNK_SIZE *sizeof(int);
  cr.end = pos;
  fsync(fsid); 
  //write new cr
  lseek(fsid, 0, SEEK_SET);
  
  if(write(fsid, &cr, sizeof(cr_t)) < 0) return -1;
  fsync(fsid);
  return 0;
}

int fsi_read(int inum, char *buffer, int block)
{
  inode_t node; 
  if (get_inode(inum, &node) < 0) return -1;
  if (get_block(&node, buffer, block) < 0) return -1;
  return 0;
}

int fsi_creat(int pinum, int type, char *name)
{
  inode_t pnode;
  if(get_inode(pinum, &pnode) < 0) return -1;
  if(strlen(name) > 27) return -1;
  printf("got node\n");
  if(fsi_lookup(pinum, name) > 0) fsi_unlink(pinum, name);

  // update inode #
  int inum = -1;
  // find empty location 
  int i;
  for (i = 0; i < 4096; i++)
  {
    if(inodes[i] == -1){
      inum = i;
      break;
    }
  }
  if (inum == -1) return -1;

  dir_ent_t dir_block[128];
  printf("before finding block\n"); 
  int foundBlock = -1;
  int dirPos = -1;
  int block = 0;  
  while(foundBlock == -1 && block < NDIRECT){
    int rc = get_block(&pnode, (char *) &dir_block, block);
    if (rc == -1) return -1;
    printf("get_block\n");
    int i;
    for(i = 0; i < 128; i++)
    {
    
      printf("dir ent: %s, name: %d\n", dir_block[i].name, dir_block[i].inum);
      
      if (dir_block[i].inum == -1){
        strcpy(dir_block[i].name, name);
	dir_block[i].inum = inum;
        dirPos = i;
	foundBlock = block;
	break;
      }
    }
    block++;
  }

  if(foundBlock == -1) return -1;
  printf("found empty block\n");
  // write updated parent node and block
  lseek(fsid, cr.end, SEEK_SET);
  int pos = cr.end;
  if (write(fsid, &dir_block, LFS_BLOCK_SIZE) < 0) return -1;
  pos += LFS_BLOCK_SIZE;

  if(pnode.size < foundBlock * LFS_BLOCK_SIZE)
    pnode.size = foundBlock *LFS_BLOCK_SIZE;
  
  // record new inode position
  inodes[pinum] = pos;
  pnode.direct[foundBlock] = cr.end;

  if(write(fsid,&pnode, sizeof(inode_t)) < 0) return -1;
  pos += sizeof(inode_t);

  if(pinum/IMAP_CHUNK_SIZE != inum/IMAP_CHUNK_SIZE){ 
    cr.chunks[pinum/IMAP_CHUNK_SIZE] = pos;
    if(write(fsid,&inodes[pinum/IMAP_CHUNK_SIZE*IMAP_CHUNK_SIZE], IMAP_CHUNK_SIZE *sizeof(int)) < 0) 
    pos += IMAP_CHUNK_SIZE * sizeof(int);
  }
  
  inode_t child;
  child.type = type;
  child.size = 0;
  for (i = 0; i < NDIRECT; i++)
    child.direct[i] = -1;

  if (type == LFS_DIRECTORY){
    dir_ent_t child_dir_block[128];
    strcpy(child_dir_block[0].name, ".");
    child_dir_block[0].inum = inum;
    strcpy(child_dir_block[1].name, "..");
    child_dir_block[1].inum = pinum;
    if(write(fsid, &child_dir_block, LFS_BLOCK_SIZE) < 0) return -1; 
    child.direct[foundBlock] = pos;
    pos += LFS_BLOCK_SIZE;
    child.size = 2 * sizeof(dir_ent_t);
  }
  
  inodes[inum] = pos;
  if(write(fsid,&child,sizeof(inode_t)) < 0) return -1;
  pos += sizeof(inode_t);
  cr.chunks[inum/IMAP_CHUNK_SIZE] = pos;
  if(write(fsid,&inodes[inum/IMAP_CHUNK_SIZE*IMAP_CHUNK_SIZE], IMAP_CHUNK_SIZE *sizeof(int)) < 0) 
    return -1;
  
  pos += IMAP_CHUNK_SIZE *sizeof(int);
  cr.end = pos;
  
  //write new cr
  lseek(fsid, 0, SEEK_SET);
  
  if(write(fsid, &cr, sizeof(cr_t)) < 0) return -1;
  fsync(fsid);
  return 0;
}

int fsi_unlink(int pinum, char* name)
{
  inode_t pnode, cnode; 
  if(get_inode(pinum, &pnode) < 0) return -1;
  
  int cinum = fsi_lookup(pinum, name);
  if (cinum < 0) return -1;

  get_inode(cinum, &cnode);

  if(cnode.type == LFS_DIRECTORY)
  {
    if(cnode.size > LFS_BLOCK_SIZE) return -1;

    dir_ent_t cdir_blocks[128];
    get_block(&cnode, (char *) &cdir_blocks, 0);

    int i;
    for(i = 0; i < 128; i++)
    {
      if(cdir_blocks[i].inum != cinum || cdir_blocks[i].inum != pinum)
        return -1;
    }
  }

  int block = 0;
  dir_ent_t dir_block[128];
  int foundBlock = -1; 
  int isEmpty = 0;
  
  while(foundBlock == -1 && block < NDIRECT){
    int rc = get_block(&pnode, (char *) &dir_block, block);
    if (rc == -1) return -1;
    
    int i;
    for(i = 0; i < 128; i++)
    {
      if (strcmp(dir_block[i].name, name) == 0){
        if(i == 0) isEmpty = 1;
	foundBlock = block;
	dir_block[i].inum = -1;
	break;
      }
    }
    block++;
  }
  
  int pos = cr.end;
  lseek(fsid, pos, SEEK_SET);
  if(write(fsid,dir_block,LFS_BLOCK_SIZE) < 0) return -1;

  if(isEmpty) pnode.direct[foundBlock] = -1;
  else pnode.direct[foundBlock] = pos;
  
  pos += LFS_BLOCK_SIZE;
  if(write(fsid,&pnode, sizeof(inode_t)) < 0) return -1;

  inodes[pinum] = pos;
  inodes[cinum] = -1;
  pos += sizeof(inode_t);
  
  if(pinum/IMAP_CHUNK_SIZE != cinum/IMAP_CHUNK_SIZE){ 
    cr.chunks[pinum/IMAP_CHUNK_SIZE] = pos;
    if(write(fsid,&inodes[pinum/IMAP_CHUNK_SIZE*IMAP_CHUNK_SIZE], IMAP_CHUNK_SIZE *sizeof(int)) < 0) 
    pos += IMAP_CHUNK_SIZE * sizeof(int);
  }
  
  cr.chunks[cinum/IMAP_CHUNK_SIZE] = pos;
  if(write(fsid, &inodes[cinum/IMAP_CHUNK_SIZE*IMAP_CHUNK_SIZE], IMAP_CHUNK_SIZE * sizeof(int)) < 0)
    return -1;

  pos += IMAP_CHUNK_SIZE *sizeof(int);
  cr.end = pos;
  lseek(fsid, 0, SEEK_SET);
  if(write(fsid, &cr, sizeof(cr_t)) < 0) return -1;
  fsync(fsid);
  return 0;
}

int fsi_shutdown()
{
  return close(fsid);
}
