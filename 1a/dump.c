#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"

#define BUFSIZE (32)

void
usage(char *prog) 
{
    fprintf(stderr, "usage: %s <-i file>\n", prog);
        exit(1);
	}

	int
	main(int argc, char *argv[])
	{
	    // arguments
	        char *inFile = "/no/such/file";

		    // input params
		        int c;
			    opterr = 0;
			        while ((c = getopt(argc, argv, "i:")) != -1) {
					switch (c) {
						case 'i':
							    inFile = strdup(optarg);
							    	    break;
								    	default:
										    usage(argv[0]);
										    	}
											    }

											        // open and create output file
												    int fd = open(inFile, O_RDONLY);
												        if (fd < 0) {
														fprintf(stderr, "can't open file %s for reading\n", inFile);
														    }

														        rec_t records[BUFSIZE];
															    while (1) {	
															    	int rc;
																	if ((rc = read(fd, records, BUFSIZE * sizeof(rec_t))) == 0)
																		    break;
																		    	int i, j;
																				for (i = 0; i < (rc / sizeof(rec_t)); i++) {
																					    printf("key:%9d rec:", records[i].key);
																					    	    for (j = 0; j < NUMRECS; j++) 
																						    		printf("%3d ", records[i].record[j]);
																									    printf("\n");
																									    	}
																										    }
																										        
																											    (void) close(fd);

																											        return 0;
																												}


