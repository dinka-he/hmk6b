// 32MGET.c

// for getline and clock_gettime
#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <valgrind/callgrind.h>

#include <hiredis.h>

#define STR_(x) #x
#define STR(x) STR_(x)

#if !(defined(MYREDIS_IP) && defined(MYREDIS_PORT))
#  error "need to define MYREDIS_IP and MYREDIS_PORT"
#else
const char myredis_ip[] = STR(MYREDIS_IP);
const int myredis_port = MYREDIS_PORT;
#endif

struct line_t{
    char *lineptr;
    size_t n;
};

int main(){
#if 0
    redisContext *c = redisConnect(myredis_ip, myredis_port);
    if(c == NULL || c->err){
        if(c){
            fprintf(stderr, "%s: %s at %d\n", __FILE__, __func__, __LINE__);
        }
        else{
            fprintf(stderr, "%s: %s at %d\n", __FILE__, __func__, __LINE__);
        }
        exit(EXIT_FAILURE);
    }
#endif
    struct line_t longString = {NULL, 0};
    if(getline(&longString.lineptr, &longString.n, stdin) < 0){
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    // format for RESP simple string
    char *longStringNonRESP = strdup(longString.lineptr);
    longString.lineptr[0] = '+';
    longString.lineptr[strlen(longString.lineptr) - 2] = '\r';
    longString.lineptr[strlen(longString.lineptr) - 1] = '\n';
    memmove(longStringNonRESP, longStringNonRESP + 1, strlen(longString.lineptr) - 3);
    longStringNonRESP[strlen(longString.lineptr) - 3] = '\0';
    // time a single 32MB GET reply w/o freeReplyObject
    struct timespec start, stop;
    void *reply;
    int ret;
    redisReader* reader = redisReaderCreate();
    reader->maxbuf = 0;
    redisReaderFeed(reader, longString.lineptr, strlen(longString.lineptr));
#if 0
    redisReply *reply;
    reply = redisCommand(c, "SET %s %s", "foo", longString.lineptr);
    freeReplyObject(reply);
#endif
    CALLGRIND_START_INSTRUMENTATION;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    ret = redisReaderGetReply(reader, &reply);
#if 0
    reply = redisCommand(c, "GET %s", "foo");
#endif
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    CALLGRIND_STOP_INSTRUMENTATION;
    if(ret != REDIS_OK){
        fprintf(stderr, "redisGetReply threw error\n");
        exit(EXIT_FAILURE);
    }
    redisReaderFree(reader);
    if(strcmp(((redisReply *)reply)->str, longStringNonRESP)){
        fprintf(stderr, "reply does not match\n");
        fprintf(stderr, "got: %s\n", longString.lineptr);
        exit(EXIT_FAILURE);
    }
    freeReplyObject(reply);
    free(longStringNonRESP);
    free(longString.lineptr);
    printf("%Lf\n", (stop.tv_sec - start.tv_sec) + 
            ((stop.tv_nsec - start.tv_nsec) * powl(10, -9)));
    // cleanup
#if 0
    redisFree(c);
#endif
    return EXIT_SUCCESS;
}
