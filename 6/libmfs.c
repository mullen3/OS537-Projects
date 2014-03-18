#include <string.h>
#include <sys/select.h>
#include "udp.h"
#include "mfs.h"
#include "MFS_Msg.h"

#define BUFFER_SIZE (4096)
MFS_Msg_t buf;
int sd;
struct sockaddr_in addr, addr2;
fd_set rsds;
struct timeval tv;

int tryMessage(MFS_Msg_t message) 
{    
  int retval, rc;
  do {  
    tv.tv_sec = 30;
    tv.tv_usec = 0;
    FD_ZERO(&rsds);
    FD_SET(sd,&rsds);
    rc = UDP_Write(sd, &addr, (char *)&message, sizeof(MFS_Msg_t));
    if (rc > 0) {
      retval = select(sd+1, &rsds, NULL, NULL, &tv);
      if (retval == -1)
        perror("select()");
      else if (retval) {
        // printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
        rc = UDP_Read(sd, &addr2, (char *)&buf, sizeof(MFS_Msg_t));
        printf("CLIENT:: Type: %d Inum: %d Block: %d Message: %s\n",buf.type,buf.inum,buf.block,(char *)buf.buffer);
        return rc;
      }
      else {
        //printf("No data within five seconds.\n");
      }
    }
  } while (!retval);
  return -1;
}

int MFS_Init(char *hostname, int port) 
{
  sd = UDP_Open(0);
  if (sd < 0) return -1;
  int rc = UDP_FillSockAddr(&addr, hostname, port); 
  if (rc == 0) return rc;
  return -1;
}

int MFS_Lookup(int pinum, char *name) 
{
  MFS_Msg_t message;
  message.type = MFS_LOOKUP;
  message.inum = pinum;
  memcpy(message.buffer,name,sizeof(char *));
  int rc = tryMessage(message); 
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_LOOKUP) {
      if (response.inum < 0) return -1;
      return response.inum;
    }
  }
  return -1;
}

int MFS_Stat(int inum, MFS_Stat_t *m)
{
  MFS_Msg_t message;
  message.type = MFS_STAT;
  message.inum = inum;
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_STAT) {
      if (response.inum < 0) return -1;
        memcpy(m,response.buffer,sizeof(MFS_Stat_t));
      return 0;
    }
  }
  return -1;
}

int MFS_Write(int inum, char *buffer, int block) 
{
  MFS_Msg_t message;
  message.type = MFS_WRITE;
  message.inum = inum;
  memcpy(message.buffer,buffer,sizeof(MFS_Msg_t));
  message.block = block;
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_WRITE) {
      if (response.inum < 0) return -1;
      return 0;
    }
  }
  return -1;
}

int MFS_Read(int inum, char *buffer, int block) 
{
  MFS_Msg_t message;
  message.type = MFS_READ;
  message.inum = inum;
  message.block = block;
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_READ) {
      if (response.inum < 0) return -1;
      if (response.block < 0) return -1;
      memcpy(buffer,response.buffer,sizeof(char *));
      return 0;
    }
  }
  return -1;
}

int MFS_Creat(int pinum, int type, char *name) 
{
  if (strlen(name) > 27) return -1;
  MFS_Msg_t message;
  message.type = MFS_CREAT;
  message.inum = pinum;
  memcpy(message.buffer,name,sizeof(char *));
  message.block = type;
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_CREAT) {
      if (response.inum < 0) return -1;
      //if (response.block < 0) return -1;
      return 0;
    }
  }
  return -1;
}

int MFS_Unlink(int pinum, char *name) 
{
  MFS_Msg_t message;
  message.type = MFS_UNLINK;
  message.inum = pinum;
  memcpy(message.buffer,name,sizeof(char *));
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_UNLINK) {
      if (response.inum < 0) return -1;
      //if ((int)response.buffer == 0) return -1;
      //if (response.block < 0) return -1;
      return 0;
    }
  }
  return -1;
}
int MFS_Shutdown() 
{
  MFS_Msg_t message;
  message.type = MFS_SHUTDOWN;
  int rc = tryMessage(message);
  if (rc > 0) {
    MFS_Msg_t response = buf;
    if (response.type == MFS_SHUTDOWN) {
      if (response.inum < 0) return -1;
      return 0;
    }
  }
  return -1;
}
