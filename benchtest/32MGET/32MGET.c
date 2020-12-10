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
    struct line_t longString = {NULL, 0};
    if(getline(&longString.lineptr, &longString.n, stdin) < 0){
        perror(NULL);
        exit(EXIT_FAILURE);
    }
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
    // time a single 32MB GET reply w/o freeReplyObject
    struct timespec start, stop;
    redisReply *reply;
    reply = redisCommand(c, "SET %s %s", "foo", longString.lineptr);
    freeReplyObject(reply);
    CALLGRIND_START_INSTRUMENTATION;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    reply = redisCommand(c, "GET %s", "foo");
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    CALLGRIND_STOP_INSTRUMENTATION;
    if(strcmp(reply->str, longString.lineptr)){
        fprintf(stderr, "reply does not match\n");
        fprintf(stderr, "got: %s\n", longString.lineptr);
        exit(EXIT_FAILURE);
    }
    freeReplyObject(reply);
    printf("%Lf\n", (stop.tv_sec - start.tv_sec) + 
            ((stop.tv_nsec - start.tv_nsec) * powl(10, -9)));
    // cleanup
    redisFree(c);
    free(longString.lineptr);
    return EXIT_SUCCESS;
}
