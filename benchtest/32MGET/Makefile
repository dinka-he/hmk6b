CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -pedantic
CFLAGS += -ggdb -O3
LDFLAGS :=

.PHONY: clean

upstream_path := /home/hejohns/documentsNoSync/481/hmk6/hiredis-upstream/hiredis/
ringbuf_path := /home/hejohns/documentsNoSync/481/hmk6/hiredis-ringbuf/hiredis/

# link to upstream libhiredis
32MGET.upstream.exe: LDFLAGS += -L$(upstream_path)
32MGET.upstream.exe: LDFLAGS += -l:libhiredis.so -Wl,-rpath=$(shell pwd)
32MGET.upstream.exe: 32MGET.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

# link to libhiredis patched w/ ringbuf
32MGET.ringbuf.exe: LDFLAGS += -L$(ringbuf_path)
32MGET.ringbuf.exe: LDFLAGS += -l:libhiredis.so -Wl,-rpath=$(shell pwd)
32MGET.ringbuf.exe: 32MGET.o
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

32MGET.o: CFLAGS += -I$(upstream_path)
32MGET.o: CFLAGS += -DMYREDIS_IP=127.0.0.1 -DMYREDIS_PORT=6379
32MGET.o: 32MGET.c
	$(CC) $^ $(CFLAGS) -c

clean:
	rm *.o *.exe callgrind.* libhiredis.so.2.0.0-dev
