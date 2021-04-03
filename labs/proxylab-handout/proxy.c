#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_LINE 128
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_HOSTNAME_SIZE 128
#define MAX_PORT_SIZE 10

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";
static const char *host_hdr = "Host: %s\r\n";

int do_proxy(int fd);
void read_requesthdrs(rio_t *rp, char *hdrs);
int parse_uri(char *uri, char *hostname, char *port, char *path);
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg);
void init_hdrs(char *hdrs, char *hostname);
void init_request(char *method, char *path, char *version, char *hdrs, char *request);
void do_request(char *hostname, char *port, char *request, char *response);
void *do_thread(void *fd);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int listenfd, connfd;

    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char hostname[MAX_HOSTNAME_SIZE], port[MAX_PORT_SIZE];
    listenfd = Open_listenfd(argv[1]);
    pthread_t tha;

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);  //Accept web client
        Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAX_HOSTNAME_SIZE, port, MAX_PORT_SIZE, 0);
        printf("Acceped connection from %s:%s\n", hostname, port);
        Pthread_create(&tha, NULL, do_thread, (void *)&connfd);
    }
    return 0;
}

void *do_thread(void *arg) {
    int connfd = *(int *)(arg);
    pthread_t th = Pthread_self();
    Pthread_detach(th);
    do_proxy(connfd);
    Close(connfd);
    return NULL;
}

int do_proxy(int fd) {
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    rio_t rio;
    char headers[MAXLINE], hostname[MAX_HOSTNAME_SIZE], port[MAX_PORT_SIZE], path[MAX_LINE], request[MAXLINE], response[MAX_OBJECT_SIZE];
    //TODO memset all char array.

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return -1;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return -1;
    }
    parse_uri(uri, hostname, port, path);
    init_hdrs(headers, hostname);
    read_requesthdrs(&rio, headers);
    init_request(method, path, version, headers, request);
    do_request(hostname, port, request, response);
    Rio_writen(fd, response, MAX_OBJECT_SIZE);
    return 0;
}

void do_request(char *hostname, char *port, char *request, char *response) {
    int client_fd;
    rio_t rio;

    client_fd = Open_clientfd(hostname, port);
    Rio_writen(client_fd, request, strlen(request));
    Rio_readinitb(&rio, client_fd);
    while (Rio_readnb(&rio, response, MAX_OBJECT_SIZE) > 0) {
        continue;
    }
    printf("%s\n", response);
    return;
}

void init_hdrs(char *hdrs, char *hostname) {
    strcat(hdrs, user_agent_hdr);
    strcat(hdrs, connection_hdr);
    strcat(hdrs, proxy_connection_hdr);
    char host[MAX_LINE];
    sprintf(host, host_hdr, hostname);
    strcat(hdrs, host);
}

void init_request(char *method, char *path, char *version, char *hdrs, char *request) {
    if (strlen(path) < 1) {
        strcpy(path, "/");
    }
    strcpy(method, "GET");
    strcpy(version, "HTTP/1.0");

    sprintf(request, "%s %s %s\r\n", method, path, version);
    strcat(request, hdrs);
    strcat(request, "\r\n");
    // printf("%s", request);
}

/*
 * read_requesthdrs - read HTTP request headers
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp, char *hdrs) {
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while (strcmp(buf, "\r\n")) {  //line:netp:readhdrs:checkterm
        if (!(strncasecmp(user_agent_hdr, buf, 10) == 0 || strncasecmp(connection_hdr, buf, 10) == 0 || strncasecmp(proxy_connection_hdr, buf, 16) == 0 || strncasecmp(host_hdr, buf, 5) == 0)) {
            strcat(hdrs, buf);
        }
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
}
/* $end read_requesthdrs */

/* $begin parse_uri */
int parse_uri(char *uri, char *hostname, char *port, char *path) {
    int p;
    char temp[MAX_LINE];
    strcpy(path, "/");
    sscanf(uri, "http://%99[^:]:%99d/%99[^\n]", hostname, &p, temp);
    strcat(path, temp);
    sprintf(port, "%d", p);
    return 0;
}
/* $end parse_uri */

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg) {
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf,
            "<body bgcolor="
            "ffffff"
            ">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */
