#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>


#define BUFSIZE (32)

int
compare_records(const void * a,const void * b)
{
  rec_t * recordA = a;
  rec_t * recordB = b;
  return (recordA->key - recordB->key);
}

void
usage(char *prog) 
{
    fprintf(stderr, "Usage: fastsort -i inputfile -o outputfile\n");
    exit(1);
}

int
main (int argc, char *argv[])
{
    char *inFile  = "/no/such/file";
    char *outFile  = "/no/such/file";

// input params
    int c;
    opterr = 0;
    int optCount = 0;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
        	switch (c) {
        	case 'i':
                	inFile      = strdup(optarg);
                	optCount++;
			break;
        	case 'o':
                	outFile     = strdup(optarg);
                	optCount++;
			break;
        	default:
                	usage(argv[0]);
        	}
      }

  if (optCount != 2 || argc > 5)
  	usage(argv[0]);

  //open input file to be read
  int fd = open(inFile, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error: Cannot open file %s\n", inFile);
    exit(1);
  }


  rec_t records[BUFSIZE];
  rec_t *allRecords = NULL;
  int numRecords = 0;

  allRecords = (rec_t *) malloc(BUFSIZE*sizeof(rec_t));

  while (1) {
    int rd;
    int i;
    if ((rd = read(fd, records, BUFSIZE * sizeof(rec_t))) == 0)
      break;

    for (i = 0; i < rd / sizeof(rec_t); i++)
    {
      //allRecords[numRecords] = (rec_t *) malloc(sizeof(rec_t));
      allRecords[numRecords] = records[i];
      numRecords ++;
    }
    allRecords = (rec_t *) realloc(allRecords, (numRecords + BUFSIZE)*sizeof(rec_t));
  }

  (void) close(fd);
  qsort(allRecords, numRecords, sizeof(rec_t), compare_records);

  int fo = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
  if (fo < 0) {
    fprintf(stderr, "Error: Cannot open file %s\n", outFile);
    exit(1);
  } 

  int recordsWritten = 0;
  while(recordsWritten < numRecords){
    int i;
    int bufferLength = (numRecords - recordsWritten < 32 ? numRecords - 
        recordsWritten : BUFSIZE);

    for (i = 0; i < bufferLength; i++){
      records[i] = allRecords[recordsWritten + i];
    }
    
    write(fo, records, bufferLength * sizeof(rec_t));
    recordsWritten += bufferLength;
  }
  (void) close(fo);
  exit(0);
}
