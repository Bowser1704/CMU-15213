#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_LINE 128
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_HOSTNAME_SIZE 128
#define MAX_PORT_SIZE 10
#define MAX_HEADER_NAME_SIZE 128
#define MAX_HEADER_VALUE_SIZE 256

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct {
    char name[MAX_HEADER_NAME_SIZE];
    char value[MAX_HEADER_VALUE_SIZE];
} header;

int do_proxy(int fd);

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
	    exit(1);
    }
    int listenfd, connfd;

    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char hostname[MAX_HOSTNAME_SIZE], port[MAX_PORT_SIZE];
    listenfd = Open_listenfd(argv[1]);
    
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //Accept web client
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAX_HOSTNAME_SIZE, port, MAX_PORT_SIZE, 0);
        printf("Acceped connection from %s:%s\n", hostname, port);

        do_proxy(connfd);
    }
    return 0;
}
