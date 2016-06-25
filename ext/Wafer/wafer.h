#ifndef WAFER_H_
#define WAFER_H_

#include <arpa/inet.h>          /* inet_ntoa */
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifdef WAFER_EPOLL
#include <sys/epoll.h>
#else
#include <sys/select.h>
#endif
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#ifdef WAFER_THREADS
#include <sys/resource.h>
#include <pthread.h>
#endif

/* Colors. Why not ? */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define RESET   "\033[0m"

/* Define boolean */

#define true 1
#define false 0

#define SERVER_STRING "Server: wafer.chttpd/0.1.0\r\n"
#define ToHex(Y) (Y>='0'&&Y<='9'?Y-'0':Y-'A'+10)
#define UNDEFINED "VALUE_UNDEFINED"

/* Settings */
#define WAFER_ONE_K 1024
#define MAX_HEADERS 1024
#define MAX_BUFFER_SIZE 1024
#define MAX_DPRINTF_SIZE 64
#ifdef WAFER_MAX_CON_CONS
#define MAX_NO_FDS WAFER_MAX_CON_CONS
#else
#define MAX_NO_FDS 1024
#endif
#define MAX_METHOD_SIZE 32
#define MAX_VER_SIZE 32
#define MAX_REQUEST_SIZE 8192
#define MAX_EVENTS MAX_NO_FDS/2
#define POLL_TIMEOUT 100        /* ms */
/* Define HTTP request parsing states */
#define STATE_PRE_REQUEST 0
#define STATE_METHOD 1
#define STATE_URI 2
#define STATE_VERSION 3
#define STATE_HEADER 4
#define STATE_COMPLETE_READING 5

#define STATUS_HTTP_OK 200
#define STATUS_HTTP_NOT_FOUND 404

typedef struct struct_request {
    char *reqStr;
    size_t reqStrLen;
    char *method;
    size_t methodLen;
    char *ver;
    size_t verLen;
    char **headers;
    size_t headersLen;
} Request;

typedef struct struct_response {
    int fd;
    char **headers;
    int flags;
    int apiFlags;
    int status;
} Response;

typedef struct {
    short int state;
    char *readBuffer;
    short readBufferIdx;
    short readBufferLen;
    char *method;
    short methodIdx;
    char *uri;
    short uriIdx;
    char *ver;
    short verIdx;
    char **headers;
    short headersIdx;
    short withinHeaderIdx;
} FdData;

#define LISTENQ  1024           /* second argument to listen() */
#define MAXLINE 1024            /* max length of a line */
#define RIO_BUFSIZE 1024

#define DEFAULT_PORT 4242
#define DEFAULT_N_CHILDREN 0

/*Preprocessor abuse */
#define SIZE_OF_CHAR sizeof(char)
#define LOG_ERROR_ON(_statement_,_condition_,_message_) do { if ((_statement_)==_condition_) fprintf(stderr,_message_); } while(0)
#define LOG_ERROR_ON_NULL(_statement_,_message_) LOG_ERROR_ON(_statement_,NULL,_message_)
#define NEW(T,v) do { T * v =  malloc(sizeof(T)); } while (0)
#define WAFER_STR(X) #X
/* Globals */
int default_port;

/*Thread stuff!*/
#ifdef WAFER_THREADS
#define QUEUESIZE MAX_NO_FDS

/* select_loop stuff */
typedef struct {
    Request request;
    Response response;
    int fd;
    FdData *fdDataList;
    fd_set *pMaster;
} THREAD_DATA;

typedef struct {
#ifdef WAFER_MAX_CON_CONS
    THREAD_DATA *buf;
#else
    THREAD_DATA buf[QUEUESIZE];
#endif
    long head, tail;
    int full, empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
} queue;

pthread_mutex_t *fd_mutex;

/*Globals*/
queue *fifo;
queue *cleaner_fifo;
int socketpair_fd[2];

/*Functions*/
void farmer_thread(THREAD_DATA);
void *worker_thread(void *arg);
void cleaner_thread(void);
queue *queueInit(void);
void queueDelete(queue * q);
void queueAdd(queue * q, THREAD_DATA in);
void queueDel(queue * q, THREAD_DATA * out);
#endif

void freeHeaders(char **);
long dbgprintf(const char *format, ...);

void server(Request * request, Response * response);

#endif                          /* WAFER_H_ */
