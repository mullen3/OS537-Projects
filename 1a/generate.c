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

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-s random seed> <-n number of records> <-o output file>\n", prog);
        exit(1);
	}

	int
	main(int argc, char *argv[])
	{
	    // program assumes a 4-byte key
	        assert(sizeof(rec_t) == 128);

		    // arguments
		        int randomSeed  = 0;
			    int recordsLeft = 0;
			        char *outFile   = "/no/such/file";

				    // input params
				        int c;
					    opterr = 0;
					        while ((c = getopt(argc, argv, "n:s:o:")) != -1) {
							switch (c) {
								case 'n':
									    recordsLeft = atoi(optarg);
									    	    break;
										    	case 's':
												    randomSeed  = atoi(optarg);
												    	    break;
													    	case 'o':
															    outFile     = strdup(optarg);
															    	    break;
																    	default:
																		    usage(argv[0]);
																		    	}
																			    }

																			        // seed random number generator
																				    srand(randomSeed);

																				        // open and create output file
																					    int fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
																					        if (fd < 0) {
																							fprintf(stderr, "can't open file %s for writing\n", outFile);
																							    }

																							        // how many written so far
																								    int writtenRecords = 0;

																								        // pack records into bigger buffers, and write those out
																									    rec_t records[BUFSIZE];

																									        while (recordsLeft > 0) {
																											int howMany = 0;
																												if (recordsLeft < BUFSIZE) {
																													    // end case: write out less than BUFSIZE records
																													    	    howMany = recordsLeft;
																														    	} else {
																																    howMany = BUFSIZE;
																																    	}

																																		int i;
																																			for (i = 0; i < howMany; i++) {
																																				    writtenRecords++;
																																				    	    // fill in random key
																																					    	    records[i].key = rand() % (int)1e9;
																																						    	    // fill in random rest of records
																																							    	    int j;
																																								    	    for (j = 0; j < NUMRECS; j++) {
																																									    		records[i].record[j] = rand() % (int)1e3;
																																												    }
																																												    	}
																																														int writeSize = howMany * sizeof(rec_t);
																																															int rc = write(fd, records, writeSize);
																																																if (rc != writeSize) {
																																																	    perror("write");
																																																	    	    exit(1);
																																																		    	}
																																																				
																																																					recordsLeft -= BUFSIZE;
																																																					    }


																																																					        // ok to ignore error code here, because we're done anyhow...
																																																						    (void) close(fd);

																																																						        return 0;
																																																							}


