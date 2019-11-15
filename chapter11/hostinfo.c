/*************************************************************************
	> File Name: hostinfo.c
	> Author: 
	> Mail: 
	> Created Time: 2019年11月13日 星期三 21时20分10秒
 ************************************************************************/

/* $begin hostinfo */
#include "csapp.h"

int main(int argc, char **argv) 
{
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;

    if (argc != 2) {
	fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
	exit(0);
    }

    /* Get a list of addrinfo records */
    memset(&hints, 0, sizeof(struct addrinfo));                         
//    hints.ai_family = AF_INET;       /* IPv4 only */        //line:netp:hostinfo:family 使用IPV4
    hints.ai_socktype = SOCK_STREAM; /* Connections only */ //line:netp:hostinfo:socktype 表明是TCP流式连接
    //重点在下面一行，读取domain name 转换为addrinfo，同时hints也是addrinfo结构，其实是用来做参数的
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* Walk the list and display each IP address */
    flags = NI_NUMERICHOST; /* Display address string instead of domain name getnameinfo默认返回host中的域名，设置该标志直接返回点分十进制数字地址字符串 */
    //flags = NI_NUMERICSERV getnameinfo默认会检查/etc/service,如果可能会直接返回服务名字不是端口号，设置该标志，跳过函数查找，简单的返回端口号.
    for (p = listp; p; p = p->ai_next) {
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
//        printf("%s\n", buf2);
        printf("%s\n", buf);
    } 

    /* Clean up */
    Freeaddrinfo(listp);

    exit(0);
}
/* $end hostinfo */
