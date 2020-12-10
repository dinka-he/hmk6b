CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -pedantic
CFLAGS += -ggdb -O3
LDFLAGS :=

.PHONY: clean

# link to master libhiredis
32MGET.master.exe: LDFLAGS += /home/hejohns/documentsNoSync/481/hmk6/hiredis-upstream/hiredis/libhiredis.so
32MGET.master.exe: 32MGET.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

# link to libhiredis patched w/ ringbuf
32MGET.ringbuf.exe: LDFLAGS += /home/hejohns/documentsNoSync/481/hmk6/hiredis-ringbuf/hiredis/libhiredis.so
32MGET.ringbuf.exe: 32MGET.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

32MGET.o: CFLAGS += -I/usr/local/include/hiredis/
32MGET.o: CFLAGS += -DMYREDIS_IP=127.0.0.1 -DMYREDIS_PORT=6379
32MGET.o: 32MGET.c
	$(CC) $^ $(CFLAGS) -c

clean:
	rm *.o *.exe callgrind.*
