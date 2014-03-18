#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mfs.h"
#include "lfsi.h"
#include "udp.h"
#include "MFS_Msg.h"

#define BUFFER_SIZE (4096)
int sd;
struct sockaddr_in s;
MFS_Msg_t reply;


void Server_Error()
{
  reply.type = MFS_ERROR;
  reply.inum = -1;
}

void Server_Lookup(int pinum, char *name)
{
  reply.type = MFS_LOOKUP;
  reply.inum = fsi_lookup(pinum,name);
}

void Server_Stat(int inum, MFS_Stat_t *stat)
{
  reply.type = MFS_STAT;
  if (fsi_stat(inum,stat) < 0)
    reply.inum = -1;
  else {
    reply.inum = inum;
    memcpy(reply.buffer,stat,sizeof(MFS_Stat_t));
  }
}

void Server_Write(int inum, char *buffer, int block)
{
  reply.type = MFS_WRITE;
  if (fsi_write(inum, buffer, block) < 0)
    reply.inum = -1;
  else {
    reply.inum = inum;
    reply.block = block;
    memcpy(reply.buffer,buffer,sizeof(char *));
  }
}

void Server_Read(int inum, char *buffer, int block)
{
  reply.type = MFS_READ;
  if (fsi_read(inum, buffer, block) < 0)
    reply.inum = -1;
  else {
    reply.inum = inum;
    reply.block = block;
    memcpy(reply.buffer,buffer,sizeof(char *));
  }
}

void Server_Creat(int pinum, int type, char *name) 
{
  reply.type = MFS_CREAT;
  if (fsi_creat(pinum, type, name) < 0)
    reply.inum = -1;
  else {
    reply.inum = pinum;
    reply.block = type;
    memcpy(reply.buffer,name,sizeof(char *));
  }
}

void Server_Unlink(int pinum, char *name)
{
  reply.type = MFS_UNLINK;
  if (fsi_unlink(pinum,name) < 0)
    reply.inum = -1;
  else {
    reply.inum = pinum;
    memcpy(reply.buffer,name,sizeof(char *));
  }
}

void Server_Shutdown()
{
  reply.type = MFS_SHUTDOWN;
  if (fsi_shutdown() < 0)
    reply.inum = -1;
}

void Server_Init(int port) 
{
  sd = UDP_Open(port);
  if (sd < 0) {
    printf("Error: Server failed to initialize!");
    exit(1);
  }
  MFS_Msg_t buf;
  int rc;
  while (1) {
    rc = UDP_Read(sd, &s, (char *)&buf, sizeof(MFS_Msg_t));
    if (rc > 0) {
      printf("SERVER<< Type: %d Inum: %d Block: %d Message: %s\n",buf.type,buf.inum,buf.block,buf.buffer);
      switch (buf.type) {
        case MFS_LOOKUP:
        Server_Lookup(buf.inum,(char *)buf.buffer);    
        break;
	case MFS_STAT:
        Server_Stat(buf.inum,(MFS_Stat_t *)buf.buffer);
	break;
	case MFS_WRITE:
        Server_Write(buf.inum, (char *)buf.buffer, buf.block);
	break;
	case MFS_READ:
        Server_Read(buf.inum, (char *)buf.buffer, buf.block);
	break;
	case MFS_CREAT:
        Server_Creat(buf.inum,buf.block,(char *)buf.buffer);
	break;
        case MFS_UNLINK:
        Server_Unlink(buf.inum,(char *)buf.buffer);
	break;
	case MFS_SHUTDOWN:
        Server_Shutdown();
	break;
	default:
        Server_Error();
      }

      printf("SERVER>> Type: %d Inum: %d Block: %d Message: %s\n",buf.type,buf.inum,buf.block,buf.buffer);
      if (reply.type == MFS_SHUTDOWN) exit(0);
    }
  }
}

int main(int argc, char *argv[]) 
{
/*  if (argc != 3) {
    printf("Usage: server [portnum] [file-system-image]");
    exit(1);
  }*/
  int port = atoi(argv[1]);
  /*  if (!isdigit(port)) {
    printf("Error: Parameter [portnum] must be an integer");
    exit(1);
  }*/
  int rc = fsi_init(argv[2]);
  if (rc < 0) {
    printf("Error: File System initialization failed!");
    exit(1);
  }
  Server_Init(port);
  return 0;
}
