CC   = gcc
OPTS = -Wall -Werror

all: libmfs.so libfsi.o server client fsitester udp.o

libmfs.so: libmfs.o udp.o
	$(CC) -shared -o libmfs.so libmfs.o udp.o

libmfs.o: libmfs.c
	$(CC) $(OPTS) -c -fpic libmfs.c 

udp.o: udp.c 
	$(CC) $(OPTS) -c -fpic udp.c

libfsi.o: libfsi.c udp.h MFS_Msg.h lfsi.h
	$(CC) $(OPTS) -c -fpic libfsi.c

# this generates the target executables
server: server.o udp.o libfsi.o
	$(CC) -o server server.o udp.o libfsi.o 

client: client.o udp.o
	$(CC) -o client client.o udp.o

fsitester: libfsi.o lfsi.h MFS_Msg.h
	$(CC) -o fsitester fsitester.c libfsi.o

# this is a generic rule for .o files 
%.o: %.c 
	$(CC) $(OPTS) -c $< -o $@

clean:
	rm -f server.o udp.o client.o server client fs.img libfsi.o libmfs.so libmfs.o fsitester



