#define MFS_LOOKUP  (0)
#define MFS_STAT    (1)
#define MFS_WRITE   (2)
#define MFS_READ    (3)
#define MFS_CREAT   (4)
#define MFS_UNLINK  (5)
#define MFS_SHUTDOWN (6)
#define MFS_ERROR   (7)

typedef struct __MFS_Msg_t {
  int type; //type of MSG
  int inum; //
  int block; //only used sometimes for read/write
  char buffer[4096];
}MFS_Msg_t;
